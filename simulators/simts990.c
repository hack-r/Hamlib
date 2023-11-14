// can run this using rigctl/rigctld and socat pty devices
// gcc -o simyaesu simyaesu.c
#define _XOPEN_SOURCE 700
// since we are POSIX here we need this
#if  0
struct ip_mreq
{
    int dummy;
};
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <hamlib/rig.h>

#define BUFSIZE 256

int mysleep = 20;

float freqA = 14074000;
float freqB = 14074500;
int filternum1 = 7;
int filternum2 = 8;
int datamode = 0;
int vfo, vfo_tx, ptt, ptt_data, ptt_mic, ptt_tune;
int operatingband;
int split;
int modeMain = 2;
int modeSub = 2;
int keyspd = 20;


#if defined(WIN32) || defined(_WIN32)
int openPort(char *comport) // doesn't matter for using pts devices
{
    int fd;
    fd = open(comport, O_RDWR);

    if (fd < 0)
    {
        perror(comport);
    }

    return fd;
}

#else
int openPort(char *comport) // doesn't matter for using pts devices
{
    int fd = posix_openpt(O_RDWR);
    char *name = ptsname(fd);

    if (name == NULL)
    {
        perror("pstname");
        return -1;
    }

    printf("name=%s\n", name);

    if (fd == -1 || grantpt(fd) == -1 || unlockpt(fd) == -1)
    {
        perror("posix_openpt");
        return -1;
    }

    return fd;
}
#endif

int
getmyline(int fd, char *buf)
{
    char c;
    int i = 0;
    memset(buf, 0, BUFSIZE);
    int retval;

    while ((retval=read(fd, &c, 1)) > 0)
    {
        buf[i++] = c;

        if (c == ';') { return strlen(buf); }
    }
    if (retval != 0)
    {
        perror("read failed:");
        close(fd);
        fd = openPort("");
    }

    return strlen(buf);
}



int main(int argc, char *argv[])
{
    char buf[256];
    char *pbuf;
    int fd = openPort(argv[1]);
    int freqa = 14074000, freqb = 140735000;
    int modeA = 1, modeB = 2;

    while (1)
    {
        hl_usleep(10);
        buf[0] = 0;

        if (getmyline(fd, buf) > 0) { printf("Cmd:%s\n", buf); }

//        else { return 0; }

        if (strncmp(buf, "RM2", 3) == 0)
        {
            pbuf = "RM20020;";
            write(fd, pbuf, strlen(pbuf));
        }
        else if (strcmp(buf, "RM5;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
            pbuf = "RM5100000;";
            write(fd, pbuf, strlen(pbuf));
        }

        else if (strcmp(buf, "AN0;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
            pbuf = "AN030;";
            write(fd, pbuf, strlen(pbuf));
        }
        else if (strcmp(buf, "IF;") == 0)
        {
            char ifbuf[256];
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
//            pbuf = "IF000503130001000+0000000000030000000;"
            sprintf(ifbuf, "IF%011d1000+0000002000000000000;", freqa);
            //pbuf = "IF00010138698     +00000000002000000 ;
            write(fd, ifbuf, strlen(ifbuf));
            continue;
        }
        else if (strcmp(buf, "NB;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "NB0;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "RA;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "RA01;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "RG;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "RG055;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "MG;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "MG050;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "AG;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "AG100;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "FV;") == 0)
        {
            hl_usleep(mysleep * 1000);
            pbuf = "FV1.2;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strncmp(buf, "IS;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "IS+0000;");
            write(fd, buf, strlen(buf));
            printf("%s\n", buf);
            continue;
        }
        else if (strncmp(buf, "IS", 2) == 0)
        {
            continue;
        }
        else if (strncmp(buf, "SM;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "SM0035;");
            write(fd, buf, strlen(buf));
            printf("%s\n", buf);
            continue;
        }
        else if (strncmp(buf, "PC;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "PC100;");
            write(fd, buf, strlen(buf));
            printf("%s\n", buf);
            continue;
        }
        else if (strcmp(buf, "FW;") == 0)
        {
            //usleep(mysleep * 1000);
            pbuf = "FW240";
            write(fd, pbuf, strlen(pbuf));
            hl_usleep(20 * 1000);
            pbuf = "0;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strncmp(buf, "FW", 2) == 0)
        {
            continue;
        }
        else if (strcmp(buf, "ID;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
            int id = 24;
            SNPRINTF(buf, sizeof(buf), "ID%03d;", id);
            write(fd, buf, strlen(buf));
            continue;
        }

#if 0
        else if (strncmp(buf, "AI", 2) == 0)
        {
            if (strcmp(buf, "AI;"))
            {
                printf("%s\n", buf);
                hl_usleep(mysleep * 1000);
                n = fprintf(fp, "%s", "AI0;");
            }
        }

#endif
        else if (strcmp(buf, "VS;") == 0)
        {
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
            pbuf = "VS0;";
            write(fd, pbuf, strlen(pbuf));
            continue;
        }
        else if (strcmp(buf, "EX032;") == 0)
        {
            static int ant = 0;
            ant = (ant + 1) % 3;
            printf("%s\n", buf);
            hl_usleep(mysleep * 1000);
            SNPRINTF(buf, sizeof(buf), "EX032%1d;", ant);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "EX", 2) == 0)
        {
            continue;
        }
        else if (strcmp(buf, "FA;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FA%011d;", freqa);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strcmp(buf, "FB;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FB%011d;", freqb);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "FA", 2) == 0)
        {
            sscanf(buf, "FA%d", &freqa);
            continue;
        }
        else if (strncmp(buf, "FB", 2) == 0)
        {
            sscanf(buf, "FB%d", &freqb);
            continue;
        }
        else if (strncmp(buf, "AI;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "AI0;");
            write(fd, buf, strlen(buf));
            continue;
        }

        else if (strncmp(buf, "PS;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "PS1;");
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "SA;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "SA0;");
            write(fd, buf, strlen(buf));
        }
        else if (buf[3] == ';' && strncmp(buf, "SF", 2) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "SF%c%011.0f%c;", buf[2],
                     buf[2] == '0' ? freqA : freqB,
                     buf[2] == '0' ? modeA + '0' : modeB + '0');
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "SF", 2) == 0)
        {
            mode_t tmpmode = buf[14];

            if (buf[2] == '0') { modeA = tmpmode - '0'; }
            else { modeB = tmpmode - '0'; }

            printf("modeA=%c, modeB=%c\n", modeA, modeB);

            continue;
        }
        else if (strncmp(buf, "MD;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "MD%d;",
                     modeA); // not worried about modeB yet for simulator
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "MD", 2) == 0)
        {
            sscanf(buf, "MD%d", &modeA); // not worried about modeB yet for simulator
            continue;
        }
        else if (strncmp(buf, "FL;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FL%03d%03d;", filternum1, filternum2);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "FL", 2) == 0)
        {
            sscanf(buf, "FL%3d%3d", &filternum1, &filternum2);
            continue;
        }
        else if (strcmp(buf, "FR;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FR%d;", vfo);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "FR", 2) == 0)
        {
            sscanf(buf, "FR%d", &vfo);
        }
        else if (strcmp(buf, "FT;") == 0)
        {
            SNPRINTF(buf, sizeof(buf), "FR%d;", vfo_tx);
            write(fd, buf, strlen(buf));
            continue;
        }
        else if (strncmp(buf, "FT", 2) == 0)
        {
            sscanf(buf, "FT%d", &vfo_tx);
        }
        else if (strncmp(buf, "DA;", 3) == 0)
        {
            SNPRINTF(buf, sizeof(buf), "DA%d;", datamode);
            write(fd, buf, strlen(buf));
            printf("%s\n", buf);
            continue;
        }
        else if (strncmp(buf, "DA", 2) == 0)
        {
            sscanf(buf, "DA%d", &datamode);
            printf("%s\n", buf);
            continue;
        }
        else if (strncmp(buf, "BD;", 3) == 0)
        {
            continue;
        }
        else if (strncmp(buf, "BU;", 3) == 0)
        {
            continue;
        }
        else if (strncmp(buf, "TX", 2) == 0)
        {
            ptt = ptt_mic = ptt_data = ptt_tune = 0;

            switch (buf[2])
            {
            case ';': ptt = 1;

            case '0': ptt_mic = 1;

            case '1': ptt_data = 1;

            case '2': ptt_tune = 1;
            }

            continue;
        }
        else if (strncmp(buf, "CB;", 3) == 0)
        {
            sprintf(buf, "CB%d;", operatingband);
            write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "CB", 2) == 0)
        {
            sscanf(buf, "CB%d", &operatingband);
        }
        else if (strncmp(buf, "TB;", 3) == 0)
        {
            sprintf(buf, "TB%d;", split);
            write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "TB", 2) == 0)
        {
            sscanf(buf, "TB%d", &split);
        }
        else if (strncmp(buf, "KS;", 3) == 0)
        {
            sprintf(buf, "KS%d;", keyspd);
            write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "KS", 2) == 0)
        {
            sscanf(buf, "KS%03d", &keyspd);
        }
        else if (strncmp(buf, "OM0;", 4) == 0)
        {
            sprintf(buf, "OM0%d;", modeMain);
            write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "OM0", 3) == 0)
        {
            sscanf(buf, "OM0%d", &modeMain);
        }
        else if (strncmp(buf, "OM1;", 4) == 0)
        {
            sprintf(buf, "OM1%d;", modeSub);
            write(fd, buf, strlen(buf));
        }
        else if (strncmp(buf, "OM1", 3) == 0)
        {
            sscanf(buf, "OM1%d", &modeSub);
        }
        else if (strlen(buf) > 0)
        {
            fprintf(stderr, "Unknown command: %s\n", buf);
        }


    }

    return 0;
}
