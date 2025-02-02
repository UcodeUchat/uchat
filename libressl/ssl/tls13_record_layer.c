/* $OpenBSD: tls13_record_layer.c,v 1.47 2020/05/29 17:54:58 jsing Exp $ */
/*
 * Copyright (c) 2018, 2019 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ssl_locl.h"

#include "tls13_internal.h"
#include "tls13_record.h"

static ssize_t tls13_record_layer_write_chunk(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *buf, size_t n);
static ssize_t tls13_record_layer_write_record(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len);

struct tls13_record_layer {
	uint16_t legacy_version;

	int ccs_allowed;
	int ccs_seen;
	int ccs_sent;
	int handshake_completed;
	int legacy_alerts_allowed;
	int phh;
	int phh_retry;

	/*
	 * Read and/or write channels are closed due to an alert being
	 * sent or received. In the case of an error alert both channels
	 * are closed, whereas in the case of a close notify only one
	 * channel is closed.
	 */
	int read_closed;
	int write_closed;

	struct tls13_record *rrec;

	struct tls13_record *wrec;
	uint8_t wrec_content_type;
	size_t wrec_appdata_len;
	size_t wrec_content_len;

	/* Pending alert messages. */
	uint8_t *alert_data;
	size_t alert_len;
	uint8_t alert_level;
	uint8_t alert_desc;

	/* Pending post-handshake handshake messages (RFC 8446, section 4.6). */
	CBS phh_cbs;
	uint8_t *phh_data;
	size_t phh_len;

	/* Buffer containing plaintext from opened records. */
	uint8_t rbuf_content_type;
	uint8_t *rbuf;
	size_t rbuf_len;
	CBS rbuf_cbs;

	/* Record protection. */
	const EVP_MD *hash;
	const EVP_AEAD *aead;
	EVP_AEAD_CTX read_aead_ctx;
	EVP_AEAD_CTX write_aead_ctx;
	struct tls13_secret read_iv;
	struct tls13_secret write_iv;
	struct tls13_secret read_nonce;
	struct tls13_secret write_nonce;
	uint8_t read_seq_num[TLS13_RECORD_SEQ_NUM_LEN];
	uint8_t write_seq_num[TLS13_RECORD_SEQ_NUM_LEN];

	/* Callbacks. */
	struct tls13_record_layer_callbacks cb;
	void *cb_arg;
};

static void
tls13_record_layer_rbuf_free(struct tls13_record_layer *rl)
{
	CBS_init(&rl->rbuf_cbs, NULL, 0);
	freezero(rl->rbuf, rl->rbuf_len);
	rl->rbuf = NULL;
	rl->rbuf_len = 0;
	rl->rbuf_content_type = 0;
}

static void
tls13_record_layer_rrec_free(struct tls13_record_layer *rl)
{
	tls13_record_free(rl->rrec);
	rl->rrec = NULL;
}

static void
tls13_record_layer_wrec_free(struct tls13_record_layer *rl)
{
	tls13_record_free(rl->wrec);
	rl->wrec = NULL;
}

struct tls13_record_layer *
tls13_record_layer_new(const struct tls13_record_layer_callbacks *callbacks,
    void *cb_arg)
{
	struct tls13_record_layer *rl;

	if ((rl = calloc(1, sizeof(struct tls13_record_layer))) == NULL)
		return NULL;

	rl->legacy_version = TLS1_2_VERSION;
	rl->cb = *callbacks;
	rl->cb_arg = cb_arg;

	return rl;
}

void
tls13_record_layer_free(struct tls13_record_layer *rl)
{
	if (rl == NULL)
		return;

	tls13_record_layer_rbuf_free(rl);

	tls13_record_layer_rrec_free(rl);
	tls13_record_layer_wrec_free(rl);

	EVP_AEAD_CTX_cleanup(&rl->read_aead_ctx);
	EVP_AEAD_CTX_cleanup(&rl->write_aead_ctx);

	freezero(rl->read_iv.data, rl->read_iv.len);
	freezero(rl->write_iv.data, rl->write_iv.len);
	freezero(rl->read_nonce.data, rl->read_nonce.len);
	freezero(rl->write_nonce.data, rl->write_nonce.len);

	freezero(rl, sizeof(struct tls13_record_layer));
}

void
tls13_record_layer_rbuf(struct tls13_record_layer *rl, CBS *cbs)
{
	CBS_dup(&rl->rbuf_cbs, cbs);
}

static const uint8_t tls13_max_seq_num[TLS13_RECORD_SEQ_NUM_LEN] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

int
tls13_record_layer_inc_seq_num(uint8_t *seq_num)
{
	int i;

	/* RFC 8446 section 5.3 - sequence numbers must not wrap. */
	if (memcmp(seq_num, tls13_max_seq_num, TLS13_RECORD_SEQ_NUM_LEN) == 0)
		return 0;

	for (i = TLS13_RECORD_SEQ_NUM_LEN - 1; i >= 0; i--) {
		if (++seq_num[i] != 0)
			break;
	}

	return 1;
}

static int
tls13_record_layer_update_nonce(struct tls13_secret *nonce,
    struct tls13_secret *iv, uint8_t *seq_num)
{
	ssize_t i, j;

	if (nonce->len != iv->len)
		return 0;

	/*
	 * RFC 8446 section 5.3 - sequence number is zero padded and XOR'd
	 * with the IV to produce a per-record nonce. The IV will also be
	 * at least 8-bytes in length.
	 */
	for (i = nonce->len - 1, j = TLS13_RECORD_SEQ_NUM_LEN - 1; i >= 0; i--, j--)
		nonce->data[i] = iv->data[i] ^ (j >= 0 ? seq_num[j] : 0);

	return 1;
}

void
tls13_record_layer_allow_ccs(struct tls13_record_layer *rl, int allow)
{
	rl->ccs_allowed = allow;
}

void
tls13_record_layer_allow_legacy_alerts(struct tls13_record_layer *rl, int allow)
{
	rl->legacy_alerts_allowed = allow;
}

void
tls13_record_layer_set_aead(struct tls13_record_layer *rl,
    const EVP_AEAD *aead)
{
	rl->aead = aead;
}

void
tls13_record_layer_set_hash(struct tls13_record_layer *rl,
    const EVP_MD *hash)
{
	rl->hash = hash;
}

void
tls13_record_layer_set_legacy_version(struct tls13_record_layer *rl,
    uint16_t version)
{
	rl->legacy_version = version;
}

void
tls13_record_layer_handshake_completed(struct tls13_record_layer *rl)
{
	rl->handshake_completed = 1;
}

void
tls13_record_layer_set_retry_after_phh(struct tls13_record_layer *rl, int retry)
{
	rl->phh_retry = retry;
}

static ssize_t
tls13_record_layer_process_alert(struct tls13_record_layer *rl)
{
	uint8_t alert_level, alert_desc;
	ssize_t ret = TLS13_IO_FAILURE;

	/*
	 * RFC 8446 - sections 5.1 and 6.
	 *
	 * A TLSv1.3 alert record can only contain a single alert - this means
	 * that processing the alert must consume all of the record. The alert
	 * will result in one of three things - continuation (user_cancelled),
	 * read channel closure (close_notify) or termination (all others).
	 */
	if (rl->rbuf == NULL)
		return TLS13_IO_FAILURE;

	if (rl->rbuf_content_type != SSL3_RT_ALERT)
		return TLS13_IO_FAILURE;

	if (!CBS_get_u8(&rl->rbuf_cbs, &alert_level))
		return tls13_send_alert(rl, TLS13_ALERT_DECODE_ERROR);

	if (!CBS_get_u8(&rl->rbuf_cbs, &alert_desc))
		return tls13_send_alert(rl, TLS13_ALERT_DECODE_ERROR);

	if (CBS_len(&rl->rbuf_cbs) != 0)
		return tls13_send_alert(rl, TLS13_ALERT_DECODE_ERROR);

	tls13_record_layer_rbuf_free(rl);

	/*
	 * Alert level is ignored for closure alerts (RFC 8446 section 6.1),
	 * however for error alerts (RFC 8446 section 6.2), the alert level
	 * must be specified as fatal.
	 */
	if (alert_desc == TLS13_ALERT_CLOSE_NOTIFY) {
		rl->read_closed = 1;
		ret = TLS13_IO_EOF;
	} else if (alert_desc == TLS13_ALERT_USER_CANCELED) {
		/* Ignored at the record layer. */
		ret = TLS13_IO_WANT_RETRY;
	} else if (alert_level == TLS13_ALERT_LEVEL_FATAL) {
		rl->read_closed = 1;
		rl->write_closed = 1;
		ret = TLS13_IO_ALERT;
	} else if (rl->legacy_alerts_allowed &&
	    alert_level == TLS13_ALERT_LEVEL_WARNING) {
		/* Ignored and not passed to the callback. */
		return TLS13_IO_WANT_RETRY;
	} else {
		return tls13_send_alert(rl, TLS13_ALERT_ILLEGAL_PARAMETER);
	}

	rl->cb.alert_recv(alert_desc, rl->cb_arg);

	return ret;
}

static ssize_t
tls13_record_layer_send_alert(struct tls13_record_layer *rl)
{
	ssize_t ret;

	/* This has to fit into a single record, per RFC 8446 section 5.1. */
	if ((ret = tls13_record_layer_write_record(rl, SSL3_RT_ALERT,
	    rl->alert_data, rl->alert_len)) != rl->alert_len) {
		if (ret == TLS13_IO_EOF)
			ret = TLS13_IO_ALERT;
		return ret;
	}

	freezero(rl->alert_data, rl->alert_len);
	rl->alert_data = NULL;
	rl->alert_len = 0;

	if (rl->alert_desc == TLS13_ALERT_CLOSE_NOTIFY) {
		rl->write_closed = 1;
		ret = TLS13_IO_SUCCESS;
	} else if (rl->alert_desc == TLS13_ALERT_USER_CANCELED) {
		/* Ignored at the record layer. */
		ret = TLS13_IO_SUCCESS;
	} else {
		rl->read_closed = 1;
		rl->write_closed = 1;
		ret = TLS13_IO_ALERT;
	}

	rl->cb.alert_sent(rl->alert_desc, rl->cb_arg);

	return ret;
}

static ssize_t
tls13_record_layer_send_phh(struct tls13_record_layer *rl)
{
	ssize_t ret;

	/* Push out pending post-handshake handshake messages. */
	if ((ret = tls13_record_layer_write_chunk(rl, SSL3_RT_HANDSHAKE,
	    CBS_data(&rl->phh_cbs), CBS_len(&rl->phh_cbs))) <= 0)
		return ret;
	if (!CBS_skip(&rl->phh_cbs, ret))
		return TLS13_IO_FAILURE;
	if (CBS_len(&rl->phh_cbs) != 0)
		return TLS13_IO_WANT_RETRY;

	freezero(rl->phh_data, rl->phh_len);
	rl->phh_data = NULL;
	rl->phh_len = 0;

	CBS_init(&rl->phh_cbs, rl->phh_data, rl->phh_len);

	rl->cb.phh_sent(rl->cb_arg);

	return TLS13_IO_SUCCESS;
}

ssize_t
tls13_record_layer_send_pending(struct tls13_record_layer *rl)
{
	/*
	 * If an alert is pending, then it needs to be sent. However,
	 * if we're already part of the way through sending post-handshake
	 * handshake messages, then we need to finish that first...
	 */

	if (rl->phh_data != NULL && CBS_len(&rl->phh_cbs) != rl->phh_len)
		return tls13_record_layer_send_phh(rl);

	if (rl->alert_data != NULL)
		return tls13_record_layer_send_alert(rl);

	if (rl->phh_data != NULL)
		return tls13_record_layer_send_phh(rl);

	return TLS13_IO_SUCCESS;
}

static ssize_t
tls13_record_layer_alert(struct tls13_record_layer *rl,
    uint8_t alert_level, uint8_t alert_desc)
{
	CBB cbb;

	if (rl->alert_data != NULL)
		return TLS13_IO_FAILURE;

	if (!CBB_init(&cbb, 0))
		goto err;

	if (!CBB_add_u8(&cbb, alert_level))
		goto err;
	if (!CBB_add_u8(&cbb, alert_desc))
		goto err;
	if (!CBB_finish(&cbb, &rl->alert_data, &rl->alert_len))
		goto err;

	rl->alert_level = alert_level;
	rl->alert_desc = alert_desc;

	return tls13_record_layer_send_pending(rl);

 err:
	CBB_cleanup(&cbb);

	return TLS13_IO_FAILURE;
}

ssize_t
tls13_record_layer_phh(struct tls13_record_layer *rl, CBS *cbs)
{
	if (rl->phh_data != NULL)
		return TLS13_IO_FAILURE;

	if (!CBS_stow(cbs, &rl->phh_data, &rl->phh_len))
		return TLS13_IO_FAILURE;

	CBS_init(&rl->phh_cbs, rl->phh_data, rl->phh_len);

	return tls13_record_layer_send_pending(rl);
}

static int
tls13_record_layer_set_traffic_key(const EVP_AEAD *aead, EVP_AEAD_CTX *aead_ctx,
    const EVP_MD *hash, struct tls13_secret *iv, struct tls13_secret *nonce,
    struct tls13_secret *traffic_key)
{
	struct tls13_secret context = { .data = "", .len = 0 };
	struct tls13_secret key = { .data = NULL, .len = 0 };
	int ret = 0;

	freezero(iv->data, iv->len);
	iv->data = NULL;
	iv->len = 0;

	freezero(nonce->data, nonce->len);
	nonce->data = NULL;
	nonce->len = 0;

	if ((iv->data = calloc(1, EVP_AEAD_nonce_length(aead))) == NULL)
		goto err;
	iv->len = EVP_AEAD_nonce_length(aead);

	if ((nonce->data = calloc(1, EVP_AEAD_nonce_length(aead))) == NULL)
		goto err;
	nonce->len = EVP_AEAD_nonce_length(aead);

	if ((key.data = calloc(1, EVP_AEAD_key_length(aead))) == NULL)
		goto err;
	key.len = EVP_AEAD_key_length(aead);

	if (!tls13_hkdf_expand_label(iv, hash, traffic_key, "iv", &context))
		goto err;
	if (!tls13_hkdf_expand_label(&key, hash, traffic_key, "key", &context))
		goto err;

	if (!EVP_AEAD_CTX_init(aead_ctx, aead, key.data, key.len,
	    EVP_AEAD_DEFAULT_TAG_LENGTH, NULL))
		goto err;

	ret = 1;

 err:
	freezero(key.data, key.len);

	return ret;
}

int
tls13_record_layer_set_read_traffic_key(struct tls13_record_layer *rl,
    struct tls13_secret *read_key)
{
	memset(rl->read_seq_num, 0, TLS13_RECORD_SEQ_NUM_LEN);

	return tls13_record_layer_set_traffic_key(rl->aead, &rl->read_aead_ctx,
	    rl->hash, &rl->read_iv, &rl->read_nonce, read_key);
}

int
tls13_record_layer_set_write_traffic_key(struct tls13_record_layer *rl,
    struct tls13_secret *write_key)
{
	memset(rl->write_seq_num, 0, TLS13_RECORD_SEQ_NUM_LEN);

	return tls13_record_layer_set_traffic_key(rl->aead, &rl->write_aead_ctx,
	    rl->hash, &rl->write_iv, &rl->write_nonce, write_key);
}

static int
tls13_record_layer_open_record_plaintext(struct tls13_record_layer *rl)
{
	CBS cbs;

	if (rl->aead != NULL)
		return 0;

	/*
	 * We're still operating in plaintext mode, so just copy the
	 * content from the record to the plaintext buffer.
	 */
	if (!tls13_record_content(rl->rrec, &cbs))
		return 0;

	tls13_record_layer_rbuf_free(rl);

	if (!CBS_stow(&cbs, &rl->rbuf, &rl->rbuf_len))
		return 0;

	rl->rbuf_content_type = tls13_record_content_type(rl->rrec);

	CBS_init(&rl->rbuf_cbs, rl->rbuf, rl->rbuf_len);

	return 1;
}

static int
tls13_record_layer_open_record_protected(struct tls13_record_layer *rl)
{
	CBS header, enc_record;
	uint8_t *content = NULL;
	ssize_t content_len = 0;
	uint8_t content_type;
	size_t out_len;

	if (rl->aead == NULL)
		goto err;

	if (!tls13_record_header(rl->rrec, &header))
		goto err;
	if (!tls13_record_content(rl->rrec, &enc_record))
		goto err;

	if ((content = calloc(1, CBS_len(&enc_record))) == NULL)
		goto err;
	content_len = CBS_len(&enc_record);

	if (!tls13_record_layer_update_nonce(&rl->read_nonce, &rl->read_iv,
	    rl->read_seq_num))
		goto err;

	if (!EVP_AEAD_CTX_open(&rl->read_aead_ctx,
	    content, &out_len, content_len,
	    rl->read_nonce.data, rl->read_nonce.len,
	    CBS_data(&enc_record), CBS_len(&enc_record),
	    CBS_data(&header), CBS_len(&header)))
		goto err;

	if (out_len > TLS13_RECORD_MAX_INNER_PLAINTEXT_LEN)
		goto err;

	if (!tls13_record_layer_inc_seq_num(rl->read_seq_num))
		goto err;

	/*
	 * The real content type is hidden at the end of the record content and
	 * it may be followed by padding that consists of one or more zeroes.
	 * Time to hunt for that elusive content type!
	 */
	/* XXX - CBS from end? CBS_get_end_u8()? */
	content_len = out_len - 1;
	while (content_len >= 0 && content[content_len] == 0)
		content_len--;
	if (content_len < 0)
		goto err;
	if (content_len > TLS13_RECORD_MAX_PLAINTEXT_LEN)
		goto err;
	content_type = content[content_len];

	tls13_record_layer_rbuf_free(rl);

	rl->rbuf_content_type = content_type;
	rl->rbuf = content;
	rl->rbuf_len = content_len;

	CBS_init(&rl->rbuf_cbs, rl->rbuf, rl->rbuf_len);

	return 1;

 err:
	freezero(content, content_len);

	return 0;
}

static int
tls13_record_layer_open_record(struct tls13_record_layer *rl)
{
	if (rl->handshake_completed && rl->aead == NULL)
		return 0;

	if (rl->aead == NULL)
		return tls13_record_layer_open_record_plaintext(rl);

	return tls13_record_layer_open_record_protected(rl);
}

static int
tls13_record_layer_seal_record_plaintext(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len)
{
	uint8_t *data = NULL;
	size_t data_len = 0;
	CBB cbb, body;

	/*
	 * Allow dummy CCS messages to be sent in plaintext even when
	 * record protection has been engaged, as long as the handshake
	 * has not yet completed.
	 */
	if (rl->handshake_completed)
		return 0;
	if (rl->aead != NULL && content_type != SSL3_RT_CHANGE_CIPHER_SPEC)
		return 0;

	/*
	 * We're still operating in plaintext mode, so just copy the
	 * content into the record.
	 */
	if (!CBB_init(&cbb, TLS13_RECORD_HEADER_LEN + content_len))
		goto err;

	if (!CBB_add_u8(&cbb, content_type))
		goto err;
	if (!CBB_add_u16(&cbb, rl->legacy_version))
		goto err;
	if (!CBB_add_u16_length_prefixed(&cbb, &body))
		goto err;
	if (!CBB_add_bytes(&body, content, content_len))
		goto err;

	if (!CBB_finish(&cbb, &data, &data_len))
		goto err;

	if (!tls13_record_set_data(rl->wrec, data, data_len))
		goto err;

	rl->wrec_content_len = content_len;
	rl->wrec_content_type = content_type;

	return 1;

 err:
	CBB_cleanup(&cbb);
	freezero(data, data_len);

	return 0;
}

static int
tls13_record_layer_seal_record_protected(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len)
{
	uint8_t *data = NULL, *header = NULL, *inner = NULL;
	size_t data_len = 0, header_len = 0, inner_len = 0;
	uint8_t *enc_record;
	size_t enc_record_len;
	ssize_t ret = 0;
	size_t out_len;
	CBB cbb;

	if (rl->aead == NULL)
		return 0;

	memset(&cbb, 0, sizeof(cbb));

	/* Build inner plaintext. */
	if (!CBB_init(&cbb, content_len + 1))
		goto err;
	if (!CBB_add_bytes(&cbb, content, content_len))
		goto err;
	if (!CBB_add_u8(&cbb, content_type))
		goto err;
	/* XXX - padding? */
	if (!CBB_finish(&cbb, &inner, &inner_len))
		goto err;

	if (inner_len > TLS13_RECORD_MAX_INNER_PLAINTEXT_LEN)
		goto err;

	/* XXX EVP_AEAD_max_tag_len vs EVP_AEAD_CTX_tag_len. */
	enc_record_len = inner_len + EVP_AEAD_max_tag_len(rl->aead);
	if (enc_record_len > TLS13_RECORD_MAX_CIPHERTEXT_LEN)
		goto err;

	/* Build the record header. */
	if (!CBB_init(&cbb, TLS13_RECORD_HEADER_LEN))
		goto err;
	if (!CBB_add_u8(&cbb, SSL3_RT_APPLICATION_DATA))
		goto err;
	if (!CBB_add_u16(&cbb, TLS1_2_VERSION))
		goto err;
	if (!CBB_add_u16(&cbb, enc_record_len))
		goto err;
	if (!CBB_finish(&cbb, &header, &header_len))
		goto err;

	/* Build the actual record. */
	if (!CBB_init(&cbb, TLS13_RECORD_HEADER_LEN + enc_record_len))
		goto err;
	if (!CBB_add_bytes(&cbb, header, header_len))
		goto err;
	if (!CBB_add_space(&cbb, &enc_record, enc_record_len))
		goto err;
	if (!CBB_finish(&cbb, &data, &data_len))
		goto err;

	if (!tls13_record_layer_update_nonce(&rl->write_nonce,
	    &rl->write_iv, rl->write_seq_num))
		goto err;

	/*
	 * XXX - consider a EVP_AEAD_CTX_seal_iov() that takes an iovec...
	 * this would avoid a copy since the inner would be passed as two
	 * separate pieces.
	 */
	if (!EVP_AEAD_CTX_seal(&rl->write_aead_ctx,
	    enc_record, &out_len, enc_record_len,
	    rl->write_nonce.data, rl->write_nonce.len,
	    inner, inner_len, header, header_len))
		goto err;

	if (out_len != enc_record_len)
		goto err;

	if (!tls13_record_layer_inc_seq_num(rl->write_seq_num))
		goto err;

	if (!tls13_record_set_data(rl->wrec, data, data_len))
		goto err;

	rl->wrec_content_len = content_len;
	rl->wrec_content_type = content_type;

	data = NULL;
	data_len = 0;

	ret = 1;

 err:
	CBB_cleanup(&cbb);

	freezero(data, data_len);
	freezero(header, header_len);
	freezero(inner, inner_len);

	return ret;
}

static int
tls13_record_layer_seal_record(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len)
{
	if (rl->handshake_completed && rl->aead == NULL)
		return 0;

	tls13_record_layer_wrec_free(rl);

	if ((rl->wrec = tls13_record_new()) == NULL)
		return 0;

	if (rl->aead == NULL || content_type == SSL3_RT_CHANGE_CIPHER_SPEC)
		return tls13_record_layer_seal_record_plaintext(rl,
		    content_type, content, content_len);

	return tls13_record_layer_seal_record_protected(rl, content_type,
	    content, content_len);
}

static ssize_t
tls13_record_layer_read_record(struct tls13_record_layer *rl)
{
	uint8_t content_type, ccs;
	ssize_t ret;
	CBS cbs;

	if (rl->rrec == NULL) {
		if ((rl->rrec = tls13_record_new()) == NULL)
			goto err;
	}
 
	if ((ret = tls13_record_recv(rl->rrec, rl->cb.wire_read, rl->cb_arg)) <= 0) {
		switch (ret) {
		case TLS13_IO_RECORD_VERSION:
			return tls13_send_alert(rl, SSL_AD_PROTOCOL_VERSION);
		case TLS13_IO_RECORD_OVERFLOW:
			return tls13_send_alert(rl, SSL_AD_RECORD_OVERFLOW);
		}
		return ret;
	}
 
	if (rl->legacy_version == TLS1_2_VERSION &&
	    tls13_record_version(rl->rrec) != TLS1_2_VERSION)
		return tls13_send_alert(rl, SSL_AD_PROTOCOL_VERSION);
 
	content_type = tls13_record_content_type(rl->rrec);

	/*
	 * Bag of hacks ahead... after the first ClientHello message has been
	 * sent or received and before the peer's Finished message has been
	 * received, we may receive an unencrypted ChangeCipherSpec record
	 * (see RFC 8446 section 5 and appendix D.4). This record must be
	 * ignored.
	 */
	if (content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
		if (!rl->ccs_allowed || rl->ccs_seen >= 2)
			return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);
		if (!tls13_record_content(rl->rrec, &cbs))
			return tls13_send_alert(rl, TLS13_ALERT_DECODE_ERROR);
		if (!CBS_get_u8(&cbs, &ccs))
			return tls13_send_alert(rl, TLS13_ALERT_DECODE_ERROR);
		if (ccs != 1)
			return tls13_send_alert(rl, TLS13_ALERT_ILLEGAL_PARAMETER);
		rl->ccs_seen++;
		tls13_record_layer_rrec_free(rl);
		return TLS13_IO_WANT_RETRY;
	}

	/*
	 * Once record protection is engaged, we should only receive
	 * protected application data messages (aside from the
	 * dummy ChangeCipherSpec messages, handled above).
	 */
	if (rl->aead != NULL && content_type != SSL3_RT_APPLICATION_DATA)
		return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);

	if (!tls13_record_layer_open_record(rl))
		goto err;

	tls13_record_layer_rrec_free(rl);

	/*
	 * On receiving a handshake or alert record with empty inner plaintext,
	 * we must terminate the connection with an unexpected_message alert.
	 * See RFC 8446 section 5.4.
	 */
	if (CBS_len(&rl->rbuf_cbs) == 0 &&
	    (rl->rbuf_content_type == SSL3_RT_ALERT ||
	     rl->rbuf_content_type == SSL3_RT_HANDSHAKE))
		return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);

	switch (rl->rbuf_content_type) {
	case SSL3_RT_ALERT:
		return tls13_record_layer_process_alert(rl);

	case SSL3_RT_HANDSHAKE:
		break;

	case SSL3_RT_APPLICATION_DATA:
		if (!rl->handshake_completed)
			return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);
		break;

	default:
		return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);
	}

	return TLS13_IO_SUCCESS;

 err:
	return TLS13_IO_FAILURE;
}

static ssize_t
tls13_record_layer_pending(struct tls13_record_layer *rl, uint8_t content_type)
{
	if (rl->rbuf_content_type != content_type)
		return 0;

	return CBS_len(&rl->rbuf_cbs);
}

static ssize_t
tls13_record_layer_read_internal(struct tls13_record_layer *rl,
    uint8_t content_type, uint8_t *buf, size_t n, int peek)
{
	ssize_t ret;

	if ((ret = tls13_record_layer_send_pending(rl)) != TLS13_IO_SUCCESS)
		return ret;

	if (rl->read_closed)
		return TLS13_IO_EOF;

	/* If necessary, pull up the next record. */
	if (CBS_len(&rl->rbuf_cbs) == 0) {
		if ((ret = tls13_record_layer_read_record(rl)) <= 0)
			return ret;

		/*
		 * We may have read a valid 0-byte application data record,
		 * in which case we need to read the next record.
		 */
		if (CBS_len(&rl->rbuf_cbs) == 0) {
			tls13_record_layer_rbuf_free(rl);
			return TLS13_IO_WANT_POLLIN;
		}
	}

	/*
	 * If we are in post handshake handshake mode, we may not see
	 * any record type that isn't a handshake until we are done.
	 */
	if (rl->phh && rl->rbuf_content_type != SSL3_RT_HANDSHAKE)
		return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);

	if (rl->rbuf_content_type != content_type) {
		/*
		 * Handshake content can appear as post-handshake messages (yup,
		 * the RFC reused the same content type...), which means we can
		 * be trying to read application data and need to handle a
		 * post-handshake handshake message instead...
		 */
		if (rl->rbuf_content_type == SSL3_RT_HANDSHAKE) {
			if (rl->handshake_completed) {
				rl->phh = 1;
				ret = TLS13_IO_FAILURE;

				/*
				 * The post handshake handshake
				 * receive callback is allowed to
				 * return:
				 *
				 * TLS13_IO_WANT_POLLIN ->
				 * I need more handshake data.
				 *
				 * TLS13_IO_WANT_POLLOUT -> I got the
				 * whole handshake message, and have
				 * enqueued a response
				 *
				 * TLS13_IO_SUCCESS -> I got the whole handshake,
				 * nothing more to do
				 *
				 * TLS13_IO_FAILURE -> something broke.
				 */
				if (rl->cb.phh_recv != NULL) {
					ret = rl->cb.phh_recv(
					    rl->cb_arg, &rl->rbuf_cbs);
				}

				tls13_record_layer_rbuf_free(rl);

				if (ret == TLS13_IO_WANT_POLLIN)
					return ret;

				/*
				 * leave post handshake handshake mode
				 * if we do not need more handshake data
				 */
				rl->phh = 0;

				if (ret == TLS13_IO_SUCCESS) {
					if (rl->phh_retry)
						return TLS13_IO_WANT_RETRY;

					return TLS13_IO_WANT_POLLIN;
				}

				return ret;
			}
		}

		return tls13_send_alert(rl, TLS13_ALERT_UNEXPECTED_MESSAGE);
	}

	if (n > CBS_len(&rl->rbuf_cbs))
		n = CBS_len(&rl->rbuf_cbs);

	/* XXX - CBS_memcpy? CBS_copy_bytes? */
	memcpy(buf, CBS_data(&rl->rbuf_cbs), n);

	if (!peek) {
		if (!CBS_skip(&rl->rbuf_cbs, n))
			goto err;
	}

	if (CBS_len(&rl->rbuf_cbs) == 0)
		tls13_record_layer_rbuf_free(rl);

	return n;

 err:
	return TLS13_IO_FAILURE;
}

static ssize_t
tls13_record_layer_peek(struct tls13_record_layer *rl, uint8_t content_type,
    uint8_t *buf, size_t n)
{
	ssize_t ret;

	do {
		ret = tls13_record_layer_read_internal(rl, content_type, buf, n, 1);
	} while (ret == TLS13_IO_WANT_RETRY);

	return ret;
}

static ssize_t
tls13_record_layer_read(struct tls13_record_layer *rl, uint8_t content_type,
    uint8_t *buf, size_t n)
{
	ssize_t ret;

	do {
		ret = tls13_record_layer_read_internal(rl, content_type, buf, n, 0);
	} while (ret == TLS13_IO_WANT_RETRY);

	return ret;
}

static ssize_t
tls13_record_layer_write_record(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *content, size_t content_len)
{
	ssize_t ret;

	if (rl->write_closed)
		return TLS13_IO_EOF;

	/*
	 * If we pushed out application data while handling other messages,
	 * we need to return content length on the next call.
	 */
	if (content_type == SSL3_RT_APPLICATION_DATA &&
	    rl->wrec_appdata_len != 0) {
		ret = rl->wrec_appdata_len;
		rl->wrec_appdata_len = 0;
		return ret;
	}

	/* See if there is an existing record and attempt to push it out... */
	if (rl->wrec != NULL) {
		if ((ret = tls13_record_send(rl->wrec, rl->cb.wire_write,
		    rl->cb_arg)) <= 0)
			return ret;
		tls13_record_layer_wrec_free(rl);

		if (rl->wrec_content_type == content_type) {
			ret = rl->wrec_content_len;
			rl->wrec_content_len = 0;
			rl->wrec_content_type = 0;
			return ret;
		}

		/*
		 * The only partial record type should be application data.
		 * All other cases are handled to completion.
		 */
		if (rl->wrec_content_type != SSL3_RT_APPLICATION_DATA)
			return TLS13_IO_FAILURE;
		rl->wrec_appdata_len = rl->wrec_content_len;
	}

	if (content_len > TLS13_RECORD_MAX_PLAINTEXT_LEN)
		goto err;

	if (!tls13_record_layer_seal_record(rl, content_type, content, content_len))
		goto err;

	if ((ret = tls13_record_send(rl->wrec, rl->cb.wire_write, rl->cb_arg)) <= 0)
		return ret;

	tls13_record_layer_wrec_free(rl);

	return content_len;

 err:
	return TLS13_IO_FAILURE;
}

static ssize_t
tls13_record_layer_write_chunk(struct tls13_record_layer *rl,
    uint8_t content_type, const uint8_t *buf, size_t n)
{
	if (n > TLS13_RECORD_MAX_PLAINTEXT_LEN)
		n = TLS13_RECORD_MAX_PLAINTEXT_LEN;

	return tls13_record_layer_write_record(rl, content_type, buf, n);
}

static ssize_t
tls13_record_layer_write(struct tls13_record_layer *rl, uint8_t content_type,
    const uint8_t *buf, size_t n)
{
	ssize_t ret;

	do {
		ret = tls13_record_layer_send_pending(rl);
	} while (ret == TLS13_IO_WANT_RETRY);
	if (ret != TLS13_IO_SUCCESS)
		return ret;

	do {
		ret = tls13_record_layer_write_chunk(rl, content_type, buf, n);
	} while (ret == TLS13_IO_WANT_RETRY);

	return ret;
}

static const uint8_t tls13_dummy_ccs[] = { 0x01 };

ssize_t
tls13_send_dummy_ccs(struct tls13_record_layer *rl)
{
	ssize_t ret;

	if (rl->ccs_sent)
		return TLS13_IO_FAILURE;

	if ((ret = tls13_record_layer_write(rl, SSL3_RT_CHANGE_CIPHER_SPEC,
	    tls13_dummy_ccs, sizeof(tls13_dummy_ccs))) <= 0)
		return ret;

	rl->ccs_sent = 1;

	return TLS13_IO_SUCCESS;
}

ssize_t
tls13_read_handshake_data(struct tls13_record_layer *rl, uint8_t *buf, size_t n)
{
	return tls13_record_layer_read(rl, SSL3_RT_HANDSHAKE, buf, n);
}

ssize_t
tls13_write_handshake_data(struct tls13_record_layer *rl, const uint8_t *buf,
    size_t n)
{
	return tls13_record_layer_write(rl, SSL3_RT_HANDSHAKE, buf, n);
}

ssize_t
tls13_pending_application_data(struct tls13_record_layer *rl)
{
	if (!rl->handshake_completed)
		return 0;

	return tls13_record_layer_pending(rl, SSL3_RT_APPLICATION_DATA);
}

ssize_t
tls13_peek_application_data(struct tls13_record_layer *rl, uint8_t *buf, size_t n)
{
	if (!rl->handshake_completed)
		return TLS13_IO_FAILURE;

	return tls13_record_layer_peek(rl, SSL3_RT_APPLICATION_DATA, buf, n);
}

ssize_t
tls13_read_application_data(struct tls13_record_layer *rl, uint8_t *buf, size_t n)
{
	if (!rl->handshake_completed)
		return TLS13_IO_FAILURE;

	return tls13_record_layer_read(rl, SSL3_RT_APPLICATION_DATA, buf, n);
}

ssize_t
tls13_write_application_data(struct tls13_record_layer *rl, const uint8_t *buf,
    size_t n)
{
	if (!rl->handshake_completed)
		return TLS13_IO_FAILURE;

	return tls13_record_layer_write(rl, SSL3_RT_APPLICATION_DATA, buf, n);
}

ssize_t
tls13_send_alert(struct tls13_record_layer *rl, uint8_t alert_desc)
{
	uint8_t alert_level = TLS13_ALERT_LEVEL_FATAL;
	ssize_t ret;

	if (alert_desc == TLS13_ALERT_CLOSE_NOTIFY ||
	    alert_desc == TLS13_ALERT_USER_CANCELED)
		alert_level = TLS13_ALERT_LEVEL_WARNING;

	do {
		ret = tls13_record_layer_alert(rl, alert_level, alert_desc);
	} while (ret == TLS13_IO_WANT_RETRY);

	return ret;
}
