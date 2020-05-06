#include "uchat.h"

t_package *mx_create_new_package() {
    t_package *new_package = malloc(MX_PACKAGE_SIZE);

    mx_memset(new_package, 0, MX_PACKAGE_SIZE);
    return new_package;
}

t_package *mx_copy_package(t_package *package) {
    t_package *new_package = NULL;

    if (package) {
        new_package = (t_package *)malloc(MX_PACKAGE_SIZE);
        mx_memcpy(new_package, package, MX_PACKAGE_SIZE);
    }
    return new_package;
}
