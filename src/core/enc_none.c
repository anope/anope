/* Module for encryption using MD5.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * This program is free but copyrighted software; see the file COPYING for
 * details.
 */

#include "module.h"

int plain_encrypt(const char *src,int len,char *dest,int size);
int plain_encrypt_check_len(int passlen, int bufsize);
int plain_decrypt(const char *src, char *dest, int size);
int plain_check_password(const char *plaintext, const char *password);


int AnopeInit(int argc, char **argv) {

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(ENCRYPTION);
 
    encmodule_encrypt(plain_encrypt);
    encmodule_encrypt_check_len(plain_encrypt_check_len);
    encmodule_decrypt(plain_decrypt);
    encmodule_check_password(plain_check_password);

    return MOD_CONT;
}

void AnopeFini(void) {
    encmodule_encrypt(NULL);
    encmodule_encrypt_check_len(NULL);
    encmodule_decrypt(NULL);
    encmodule_check_password(NULL);
}

int plain_encrypt(const char *src,int len,char *dest,int size) {
    if(size>=len) {
        memset(dest,0,size);
        strncpy(dest,src,len);
        dest[len] = '\0';
        return 0;
    }
    return -1;
}

int plain_encrypt_check_len(int passlen, int bufsize) {
    if(bufsize>=passlen) {
        return 0;
    }
    return bufsize;
}

int plain_decrypt(const char *src, char *dest, int size) {
    memset(dest,0,size);
    strncpy(dest,src,size);
    dest[size] = '\0';
    return 1;
}

int plain_check_password(const char *plaintext, const char *password) {
    if(strcmp(plaintext,password)==0) {
        return 1;
    }
    return 0;
}

/* EOF */

