/**
 * myfunction.c
 *
 *  Created on: 2022-03-08
 *      Author: chilianjiang
 */

#include "myfunc.h"

#define DEFAULT_INITIAL_CAPACITY  16
#define MAXIMUM_CAPACITY  1<<30
#define DEFAULT_LOAD_FACTOR 0.75

#define indexFor(h,length) (h)&(length-1)

//===========================================================
unsigned char bam_nt16_table[256]={15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,1, 2, 4, 8, 15,15,15,15, 15,15,15,15, 15, 0 /*=*/,15,15,15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15};
unsigned char anti_base_table[256]={'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'T', 'N', 'G', 'N', 'N', 'N', 'C', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'A', 'A', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'T', 'N', 'G', 'N', 'N', 'N', 'C', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'A', 'A', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'};
unsigned char bam_int_2_base[16]={'N', 'A', 'C', 'N', 'G', 'N', 'N', 'N', 'T', 'N', 'N', 'N', 'N', 'N', 'N', 'N'};
//==================== initial global variables
//thread_num=1;

//===========================================================

//==================== get hardware information

#ifdef _WIN32

char *get_uuid(){
    FILE* pipe=_popen("wmic csproduct get UUID", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "UUID", 4)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

char *get_processor_id(){
    FILE* pipe=_popen("wmic cpu get processorid", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "ProcessorId", 11)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

char *get_serial_number(){
    FILE* pipe=_popen("wmic bios get serialnumber", "r");
    if(!pipe) return NULL;

    char buff[128];

    while(fgets(buff, 128, pipe)){
        if(strncmp(buff, "SerialNumber", 12)==0){
            if(fgets(buff, 128, pipe)==NULL){
                _pclose(pipe);
                return NULL;
            }else{
                int len=chmop(buff);
                char *res=my_new(len+1, sizeof(char));
                memcpy(res, buff, len*sizeof(char));
                _pclose(pipe);
                return res;
            }
        }
    }

    _pclose(pipe);
    return NULL;
}

inline char *path_2_absolute_path(char *path){
    char *res=calloc(1024, sizeof(char));
	_fullpath(res, path, 1024);
	return res;
}

#else

char *get_uuid(){
    FILE *pipe=popen("cat /proc/sys/kernel/random/boot_id", "r");
    if(!pipe) return NULL;

    char buff[128];

    if(fgets(buff, 128, pipe)){
        int len=chmop(buff);
        char *res=my_new(len+1, sizeof(char));
        memcpy(res, buff, len*sizeof(char));
        pclose(pipe);
        return res;
    }

    return NULL;
}

char *get_processor_id(){
    /*
    int sockfd;
    struct ifreq tmp;

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0) return NULL;

    memset(&tmp,0,sizeof(struct ifreq));
    strncpy(tmp.ifr_name,"eth0",sizeof(tmp.ifr_name)-1);
    if((ioctl(sockfd,SIOCGIFHWADDR, &tmp))<0) return NULL;

    char *mac_addr=calloc(30, sizeof(char));
    sprintf(mac_addr, "%02x%02x%02x%02x%02x%02x",
            (unsigned char)tmp.ifr_hwaddr.sa_data[0],
            (unsigned char)tmp.ifr_hwaddr.sa_data[1],
            (unsigned char)tmp.ifr_hwaddr.sa_data[2],
            (unsigned char)tmp.ifr_hwaddr.sa_data[3],
            (unsigned char)tmp.ifr_hwaddr.sa_data[4],
            (unsigned char)tmp.ifr_hwaddr.sa_data[5]
    );
    close(sockfd);

    return mac_addr;
    */

    char *res=my_new(128, sizeof(char));
    strcpy(res, "not found cpuid");
    return res;
}

char *get_serial_number(){
    return NULL;
}

inline char *path_2_absolute_path(char *path){
    return realpath(path, NULL);
}

#endif // _WIN32

//==================== avx

inline void *calloc_align_ptr(int64_t size, int8_t align_bytes){
    void *base_ptr = my_new(size+64+align_bytes, 1);    //+64 is for avx512 alignment
    void *mem_ptr = (void *)(((int64_t)((int64_t)base_ptr+align_bytes-1))&~((int64_t)(align_bytes-1)));
    if(mem_ptr==base_ptr) mem_ptr=(void *)((int64_t)base_ptr+align_bytes);
    *((int8_t *)mem_ptr-1)= (int8_t)((int64_t)mem_ptr-(int64_t)base_ptr);
    return mem_ptr;
}

inline void free_align_ptr(void *ptr){free((void *)((int64_t)ptr-*((int8_t *)ptr-1)));}

//==================== time

inline char *getTime(){
    char *res=(char *)my_new(200, sizeof(char));
    struct TIMEB now;
    ftime(&now);
    struct tm *s=localtime(&(now.time));
    sprintf(res, "%04d-%02d-%02d %02d:%02d:%02d.%03d", s->tm_year+1900, s->tm_mon+1, s->tm_mday, s->tm_hour, s->tm_min, s->tm_sec, now.millitm);
    return res;
}

typedef struct _ntp_time
{
    unsigned int coarse;
    unsigned int fine;
} ntp_time;

struct ntp_packet
{
    unsigned char leap_ver_mode;
    unsigned char startum;
    char poll;
    char precision;
    int root_delay;
    int root_dispersion;
    int reference_identifier;
    ntp_time reference_timestamp;
    ntp_time originage_timestamp;
    ntp_time receive_timestamp;
    ntp_time transmit_timestamp;
};

#ifndef _WIN32

int get_ntp_time(int *res_year, int *res_month, int *res_day, int *res_hour, int *res_min, int *res_sec, int64_t *res_time){
    int NTP_PORT=123;
    char *NTP_PORT_STR="123";
    int TIME_PORT=37;
    char *NTP_SERVER_IP="cn.pool.ntp.org";
    //char *NTP_SERVER_IP="162.159.200.123";

    char *NTPV1="NTP/V1";
    char *NTPV2="NTP/V2";
    char *NTPV3="NTP/V3";
    char *NTPV4="NTP/V4";
    char *TIME="TIME/UDP";

    int NTP_PCK_LEN=48;
    int LI=0;
    int MODE=3;
    int STRATUM=0;
    int POLL=4;
    int PREC=-6;
    int64_t JAN_1970=0x83aa7e80;   /* 1900 年～1970 年之间的时间秒数 */

    int sockfd;
    struct addrinfo *addr=NULL;
    {
        int rc;
        struct addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        /*调用 getaddrinfo()函数， 获取地址信息*/
        rc = getaddrinfo(NTP_SERVER_IP, NTP_PORT_STR, &hints, &addr);
        if (rc != 0){
            perror("getaddrinfo");
            return 1;
        }

        /* 创建套接字 */
        sockfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol); //IPv4, 数据报套接字, UDP
        if (sockfd <0 ){
            perror("socket");
            return 1;
        }
    }

    struct ntp_packet ret_time;
    struct timeval block_time;
    fd_set pending_data;
    char data[NTP_PCK_LEN * 8];
    char protocol[32];
    int packet_len, data_len = addr->ai_addrlen, count = 0, result, i,re;

    /* 组织请求报文 */
    {
        char version = 1;
        long tmp_wrd;
        int port;
        time_t timer;
        strcpy(protocol, NTPV4);
        /*判断协议版本*/
        if(!strcmp(protocol, NTPV1)||!strcmp(protocol, NTPV2)||!strcmp(protocol, NTPV3)||!strcmp(protocol, NTPV4)){
            memset(data, 0, NTP_PCK_LEN);
            port = NTP_PORT;
            /*设置 16 字节的包头*/
            version = protocol[5] - 0x30;
            tmp_wrd = htonl((LI << 30)|(version << 27) \
            |(MODE << 24)|(STRATUM << 16)|(POLL << 8)|(PREC & 0xff));
            memcpy(data, &tmp_wrd, sizeof(tmp_wrd));

            /*设置 Root Delay、 Root Dispersion 和 Reference Indentifier */
            tmp_wrd = htonl(1<<16);
            memcpy(&data[4], &tmp_wrd, sizeof(tmp_wrd));
            memcpy(&data[8], &tmp_wrd, sizeof(tmp_wrd));
            /*设置 Timestamp 部分*/
            time(&timer);
            /*设置 Transmit Timestamp coarse*/
            tmp_wrd = htonl(JAN_1970 + (long)timer);
            memcpy(&data[40], &tmp_wrd, sizeof(tmp_wrd));
            /*设置 Transmit Timestamp fine*/
            tmp_wrd = htonl((long)(4294 * (timer) + ((1981 * (timer)) >> 11)));
            memcpy(&data[44], &tmp_wrd, sizeof(tmp_wrd));
            packet_len=NTP_PCK_LEN;
        }else if (!strcmp(protocol, TIME)){ /* "TIME/UDP" */
            port = TIME_PORT;
            memset(data, 0, 4);
            packet_len=4;
        }else{
            packet_len=0;
            close(sockfd);
            return 0;
        }
    }

    /*客户端给服务器端发送 NTP 协议数据包*/
    if ((result = sendto(sockfd, data, packet_len, 0, addr->ai_addr, data_len)) < 0){
        perror("sendto");
        close(sockfd);
        return 0;
    }
    /*调用select()函数，并设定超时时间为10s*/
    FD_ZERO(&pending_data);
    FD_SET(sockfd, &pending_data);
    block_time.tv_sec=10;
    block_time.tv_usec=0;
    if (select(sockfd + 1, &pending_data, NULL, NULL, &block_time) > 0){
        /*接收服务器端的信息*/
        if ((count = recvfrom(sockfd, data, NTP_PCK_LEN * 8, 0, addr->ai_addr, &data_len)) < 0){
            perror("recvfrom");
            close(sockfd);
            return 0;
        }
        if (protocol == TIME){
            memcpy(&ret_time.transmit_timestamp, data, 4);
            close(sockfd);
            return 1;
        }else if (count < NTP_PCK_LEN){
            close(sockfd);
            return 0;
        }

        /* 设置接收 NTP 包的数据结构 */
        ret_time.leap_ver_mode = ntohl(data[0]);
        ret_time.startum = ntohl(data[1]);
        ret_time.poll = ntohl(data[2]);
        ret_time.precision = ntohl(data[3]);
        ret_time.root_delay = ntohl(*(int*)&(data[4]));
        ret_time.root_dispersion = ntohl(*(int*)&(data[8]));
        ret_time.reference_identifier = ntohl(*(int*)&(data[12]));
        ret_time.reference_timestamp.coarse = ntohl(*(int*)&(data[16]));
        ret_time.reference_timestamp.fine = ntohl(*(int*)&(data[20]));
        ret_time.originage_timestamp.coarse = ntohl(*(int*)&(data[24]));
        ret_time.originage_timestamp.fine = ntohl(*(int*)&(data[28]));
        ret_time.receive_timestamp.coarse = ntohl(*(int*)&(data[32]));
        ret_time.receive_timestamp.fine = ntohl(*(int*)&(data[36]));
        ret_time.transmit_timestamp.coarse = ntohl(*(int*)&(data[40]));
        ret_time.transmit_timestamp.fine = ntohl(*(int*)&(data[44]));

        /* 将NTP时间戳转换为日期 */
        time_t currentTime = ret_time.transmit_timestamp.coarse - JAN_1970;
        struct tm *s=localtime(&(currentTime));
        *(res_year)=s->tm_year+1900;
        *(res_month)=s->tm_mon+1;
        *(res_day)=s->tm_mday;
        *(res_hour)=s->tm_hour;
        *(res_min)=s->tm_min;
        *(res_sec)=s->tm_sec;
        if(res_time) *(res_time)=currentTime;

        close(sockfd);
        return 1;
    } /* end of if select */

    close(sockfd);
    return 0;
}

inline int get_network_time(int *res_year, int *res_month, int *res_day, int *res_hour, int *res_min, int *res_sec, int64_t *res_time){get_ntp_time(res_year, res_month, res_day, res_hour, res_min, res_sec, res_time);}

#endif

//==================== string

inline void mylog(char *str){
    struct TIMEB now;
    ftime(&now);
    struct tm *s=localtime(&(now.time));
    fprintf(stdout, "%04d-%02d-%02d %02d:%02d:%02d.%03d\t%s\n", s->tm_year+1900, s->tm_mon+1, s->tm_mday, s->tm_hour, s->tm_min, s->tm_sec, now.millitm, str);
    fflush(stdout);
}

inline void myerror(char *str){
    struct TIMEB now;
    ftime(&now);
    struct tm *s=localtime(&(now.time));
    fprintf(stderr, "%04d-%02d-%02d %02d:%02d:%02d.%03d\t%s\n", s->tm_year+1900, s->tm_mon+1, s->tm_mday, s->tm_hour, s->tm_min, s->tm_sec, now.millitm, str);
    fflush(stderr);
}

inline double format_double(double d, int nBits){
    if(nBits<0) return d;
    else{
        double d1=pow(10.0, nBits);
        double d2=d>0 ? (0.5/d1):(-0.5/d1);
        int64_t l=(int64_t)((d+d2)*d1);
        return (double)l/d1;
    }
}

inline int64_t double_2_int64_by_union(double d){int64_t *res=(int64_t *)(&d);return *(res);}

int is_integer(char *str){
    if(str[0]=='-') str++;
    char c;
    while((c=*(str++))){
        if(!isdigit(c)) return 0;
    }
    return 1;
}

int is_double(char *str){
    if(str[0]=='-') str++;

    if(str[0]=='i'){
        if(str[1]=='n'){
            if(str[2]=='f'){
                if(str[3]=='\0') return 1;
                else return 0;
            }else return 0;
        }else return 0;
    }else if(str[0]=='n'){
        if(str[1]=='a'){
            if(str[2]=='n'){
                if(str[3]=='\0') return 1;
                else return 0;
            }else return 0;
        }else return 0;
    }else if(str[0]=='e' || str[0]=='E') return 0;

    int dot_num=0, e_num=0;
    char c;
    while((c=*(str++))){
        if(isdigit(c)) continue;
        else if(c=='.') dot_num++;
        else if(c=='E' || c=='e') e_num++;
        else return 0;
    }

    if(dot_num>1 || e_num>1) return 0;
    return 1;
}

inline int is_digit(char *str){
    return is_integer(str) || is_double(str);
}

int chmop(char *str){
    int i=strlen(str)-1;
    for(;i>=0;i--){
        if(str[i]!='\n' && str[i]!=' ' && str[i]!='\t' && str[i]!='\r') break;
        str[i]='\0';
    }
    return (str[0]=='\0') ? 0 : (i+1);
}

int chmop_with_len(char *str, int len){
    int i=len-1;
    for(;i>=0;i--){
        if(str[i]!='\n' && str[i]!=' ' && str[i]!='\t' && str[i]!='\r') break;
        str[i]='\0';
    }
    return (str[0]=='\0') ? 0 : (i+1);
}

void freeArray(int len, void **array){
    int i;
    for(i=0;i<len;i++) free(array[i]);
    free(array);
}

inline char *str_copy_range_of(char *str, int l_str, int from, int to, int *resLen){
    int len=(to-from)<(l_str-from) ? (to-from):(l_str-from);
    char *res=my_new2((len+1)*sizeof(char));
    memcpy(res, str+from, len);
    res[len]='\0';
    if(resLen) *(resLen)=len;
    return res;
}

inline char *substring(char *str, int64_t start, int64_t end){
    int64_t len=end-start;
    char *res=(char *)my_new((len+1), sizeof(char));
    memcpy(res, str+start, len*sizeof(char));
    return res;
}

inline char *str_2_lower_no_copy(char *str){
    int i=-1;
    while(str[++i]) str[i]=tolower(str[i]);
    return str;
}

inline char *str_2_upper_no_copy(char *str){
    int i=-1;
    while(str[++i]) str[i]=toupper(str[i]);
    return str;
}

inline char* str_2_lower_no_copy_2(char *str, int len){
    int i=-1;
    while(++i<len) str[i]=tolower(str[i]);
    return str;
}

inline char* str_2_upper_no_copy_2(char *str, int len){
    int i=-1;
    while(++i<len) str[i]=toupper(str[i]);
    return str;
}

inline char *str_2_lower_with_copy(char *str){
    return str_2_lower_with_copy_2(str, strlen(str));
}

inline char *str_2_upper_with_copy(char *str){
    return str_2_upper_with_copy_2(str, strlen(str));
}

inline char* str_2_lower_with_copy_2(char *str, int len){
    char *newStr=(char *)my_new2((len+1)*sizeof(char));
    memcpy(newStr, str, len);
    newStr[len]='\0';
    return str_2_lower_no_copy_2(newStr, len);
}

inline char* str_2_upper_with_copy_2(char *str, int len){
    char *newStr=(char *)my_new2((len+1)*sizeof(char));
    memcpy(newStr, str, len);
    newStr[len]='\0';
    return str_2_upper_no_copy_2(newStr, len);
}

int str_tab_index(char *str, char delimiter, int *tabs){
    int i, j;
    for(i=0,j=0;;i++){
        char c=str[i];
        if(c==delimiter) tabs[j++]=i;
        else if(c=='\0'){
            tabs[j++]=i;
            break;
        }
    }
    return j;
}

char **split(char *str, char delimiter, int *resNum, int *tabs){
    int i, num;

    num=str_tab_index(str, delimiter, tabs);
    *(resNum)=num;

    char **arr=my_new(num, sizeof(char *));
    arr[0]=str_copy_with_len(str, tabs[0]);
    for(i=0;i<(num-1);i++){
        arr[i+1]=str_copy_with_len(str+tabs[i]+1, tabs[i+1]-tabs[i]-1);
    }
    return arr;
}

char **split_with_no_copy(char *str, char delimiter, int *resNum, int *tabs){
    int i, num;

    num=str_tab_index(str, delimiter, tabs);
    *(resNum)=num;

    char **arr=my_new(num, sizeof(char *));
    arr[0]=str;
    for(i=0;i<(num-1);i++){
        str[tabs[i]]='\0';
        arr[i+1]=str+tabs[i]+1;
    }
    return arr;
}

inline char *str_copy(char *str){return str_copy_with_len(str, strlen(str));}

inline char *str_copy_with_len(char *str, int len){char *res=my_new2((len+1));memcpy(res, str, len);res[len]='\0';return res;}

char *str_copy_and_ptr_end(char *buff, char *str){
    while(*(str)){
        *(buff)=*(str);
        buff++;
        str++;
    }
    *(buff)='\0';
    return buff;
}

int str_starts(char *str, char *sub){
    int i=0;
    while(1){
        char c1=str[i];
        char c2=sub[i];
        if(c2=='\0') return 1;
        if(c1!=c2) return 0;
        i++;
    }
}

int str_ends(char *str, char *sub){
    int i=strlen(str)-1;
    int j=strlen(sub)-1;
    if(i<j) return 0;
    for(;i>=0&&j>=0;i--,j--){
        if(str[i]!=sub[j]) return 0;
    }
    return 1;
}

inline int str_indexOf(char *str, char *sub){
    return mem_indexOf(str, strlen(str)*sizeof(char), sub, strlen(sub)*sizeof(char));
}

inline int str_indexOf_from(char *str, char *sub, int from){
    return mem_indexOf_from(str, strlen(str)*sizeof(char), sub, strlen(sub)*sizeof(char), from);
}

int mem_indexOf(void *ptr, int64_t l_ptr, void *sub, int64_t l_sub){
    if(!ptr || !sub) return -1;

    int64_t i, max=l_ptr-l_sub;
    for(i=0;i<=max;i++){
        if(memcmp(ptr+i, sub, l_sub)==0) return i;
    }
    return -1;
}

int mem_indexOf_from(void *ptr, int64_t l_ptr, void *sub, int64_t l_sub, int64_t from){
    if(!ptr || !sub) return -1;

    int64_t i, max=l_ptr-l_sub;
    for(i=from;i<=max;i++){
        if(memcmp(ptr+i, sub, l_sub)==0) return i;
    }
    return -1;
}

char *str_replace(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep
    int len_with; // length of with
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for (count = 0; tmp = strstr(ins, rep); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = my_new2(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

inline void str_replace_char_with_no_copy(char *str, char pre, char now){while(*(str)){if(*(str)==pre) *(str)=now;str++;}}

char *str_reverse(char *str){
    int len=strlen(str), i, j;
    char *res=(char *)my_new2((len+1)*sizeof(char));

    for(i=0,j=len-1;i<len;i++,j--){
        res[i]=str[j];
    }
    res[i]='\0';

    return res;
}

//================ file operation

AList_l *get_file_list(char *path){
    DIR *dir=opendir(path);
    if(dir==NULL) return NULL;

    int l_path=strlen(path);

    AList_l *list=new_alist_l(16);

    struct dirent *ptr;
    while((ptr=readdir(dir))!=NULL){
        char *name=ptr->d_name;
        int l_name=strlen(name);
        //--
        if(strcmp(name, ".")==0 || strcmp(name, "..")==0) continue;
        else{
            if(path[l_path-1]=='/'){
                char *file=calloc(l_path+l_name+1, sizeof(char));
                memcpy(file, path, l_path*sizeof(char));
                memcpy(file+l_path, name, l_name*sizeof(char));
                alist_l_add(list, (int64_t)file);
            }else{
                char *file=calloc(l_path+l_name+2, sizeof(char));
                memcpy(file, path, l_path*sizeof(char));
                file[l_path]='/';
                memcpy(file+l_path+1, name, l_name*sizeof(char));
                alist_l_add(list, (int64_t)file);
            }
        }
    }

    closedir(dir);

    return list;
}

//return type: 1:file, 2:directory, 0:error
int get_file_type(char *path){
    struct stat s_buf;
    stat(path,&s_buf);

    if(S_ISREG(s_buf.st_mode)) return 1;
    if(S_ISDIR(s_buf.st_mode)) return 2;

    return 0;
}

inline int64_t get_file_size(char *path){
    int64_t filesize = -1;
    FILE *fp = fopen(path, "rb");
    if(fp == NULL) return filesize;
    fseek(fp, 0L, SEEK_END);
    filesize = ftell(fp);
    fclose(fp);
    return filesize;
}

inline void *read_file_content(char *path, int64_t *resSize){
    FILE *f=fopen(path, "rb");
    if(!f){
        fprintf(stderr, "file=%s, not exist!\n", path);
        *(resSize)=-1;
        return NULL;
    }

    int block_size=4096;
    SBuilder *sb= new_s_builder(block_size);
    char *buff= calloc(block_size, sizeof(char));
    while (!feof(f)) s_builder_add_str_with_len(sb, buff, fread(buff, 1, block_size, f));
    free(buff);
    fclose(f);

    *(resSize)=sb->size;
    void *res=(void *)(sb->str);
    free(sb);

    return res;
}

void write_file_content(char *path, void *buff, int64_t size){
    int64_t one_write_file_size=4096;
    FILE *f=fopen(path, "w+");
    if(!f){
        fprintf(stderr, "%s cannot be written, exit!\n", path);
        exit(0);
    }
    while(size){
        int64_t num=size<one_write_file_size ? size:one_write_file_size;
        int64_t tmp=fwrite(buff, num, 1, f);
        buff = (void *)((int64_t)buff + tmp);
        size-=tmp;
    }
    fclose(f);
}

void write_string_with_multi_lines(FILE *f, int strLen, char *str, int oneLineLen){
    int i;
    for(i=0;i<strLen;){
        fprintf(f, "%c", str[i]);
        i++;
        if((i%oneLineLen)==0) fprintf(f, "\n");
    }
    if(i%oneLineLen) fprintf(f, "\n");
}

GzStream *gz_stream_open(char *file, char *mode){
    if(file==NULL){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: file's name cannot be null.\n", buff);
        exit(0);
    }
    if(mode==NULL){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: operate file mode cannot be null.\n", buff);
        exit(0);
    }
    if(strcmp(mode, "w")!=0 && strcmp(mode, "r")!=0){
        char buff[1000];
        debug_info(buff);
        fprintf(stderr, "%s: operate mode(%s) error, it should be \"w\" or \"r\".\n", buff);
        exit(0);
    }
    //--
    GzStream *gz=my_new(1, sizeof(GzStream));
    gz->filename= str_copy(file);
    if(mode[0]=='r'){
#ifdef _use_bz2
        if(str_ends(file, ".bz2")){
            FILE *f=fopen(file, mode);
            if(f==NULL){fprintf(stderr, "open (%s) error, please check!\n", file);exit(0);}
            gz->bz2File=BZ2_bzReadOpen(&(gz->bzerror), f, 0, 0, NULL, 0);
            if(gz->bzerror!=BZ_OK){
                BZ2_bzReadClose(&(gz->bzerror), gz->bz2File);
                fprintf(stderr, "open (%s) .bz2 format error, please check!\n", file);
                exit(0);
            }
        }else
#endif // _use_bz2
        {
            gz->fp=gzopen(file, mode);
            if(gz->fp<=0){fprintf(stderr, "open (%s) error, please check!\n", file);exit(0);}
        }
        gz->is_write=0;
    }else{
        if(str_ends(file, ".gz") || str_ends(file, ".bam")){
            gz->fp=gzopen(file, mode);
            if(gz->fp<=0){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
        }
#ifdef _use_bz2
            else if(str_ends(file, ".bz2")){
            FILE *f=fopen(file, mode);
            if(f==NULL){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
            gz->bz2File=BZ2_bzWriteOpen(&(gz->bzerror), f, 9, 0, 30);
            if(gz->bzerror != BZ_OK){
                BZ2_bzWriteClose(&(gz->bzerror), gz->bz2File, 0, NULL, NULL);
                fprintf(stderr, "create (%s) .bz2 format error, please check!\n", file);
                exit(0);
            }
        }
#endif // _use_bz2
        else{
            gz->out=fopen(file, mode);
            if(gz->out<=0){fprintf(stderr, "create (%s) error, please check!\n", file);exit(0);}
        }
        gz->is_write=1;
    }
    gz->buf=my_new(gz_stream_buff_size, sizeof(char));

    return gz;
}

int gz_read_util(GzStream *gz, char delimiter, char *buff, int max_len, int *res_len){
    if(gz->is_eof && gz->begin>=gz->end) return 0;
    int len=0;
    while(1){
        if(gz->begin>=gz->end){
            gz->begin=0;
            if(gz->fp) gz->end=gzread(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) gz->end=BZ2_bzRead(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
            if(gz->end==0) gz->is_eof=1;
            else if(gz->end==-1){
                fprintf(stderr, "read file (%s) error, maybe format error!\n", gz->filename);
                exit(0);
            }
        }

        if(gz->begin<gz->end){
            char *last=gz->buf+gz->begin;
            int i, last_len=gz->end-gz->begin;
            for(i=0;i<last_len;i++){
                if(last[i]==delimiter){
                    if((len+i)>max_len){
                        fprintf(stderr, "[error in reading file]: length(read_once_seq) larger than %d\n", max_len);
                        exit(0);
                    }else{
                        gz->begin+=i+1;
                        memcpy(buff+len, last, i*sizeof(char));
                        len+=i;
                        buff[len]='\0';
                        *(res_len)=len;
                        return 1;
                    }
                }
            }
            if((len+i)>max_len){
                fprintf(stderr, "[error in reading file]: length(read_once_seq) larger than %d\n", max_len);
                exit(0);
            }else{
                gz->begin+=i;
                memcpy(buff+len, last, i*sizeof(char));
                len+=i;
            }
        }

        if(gz->is_eof){
            if(len==0){
                return 0;
            }else{
                buff[len]='\0';
                *(res_len)=len;
                return 1;
            }
        }
    }
}

int gz_read(GzStream *gz, char *str, int len){
    if(gz->is_eof && gz->begin>=gz->end) return 0;
    int res_len=0;
    while(1){
        if(gz->begin>=gz->end){
            gz->begin=0;
            if(gz->fp) gz->end=gzread(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) gz->end=BZ2_bzRead(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
            if(gz->end==0) gz->is_eof=1;
            else if(gz->end==-1){
                fprintf(stderr, "read file (%s) error, maybe format error!\n", gz->filename);
                exit(0);
            }
        }
        int last=gz->end-gz->begin;
        if(len<=last){
            memcpy(str, gz->buf+gz->begin, len);
            gz->begin+=len;
            res_len+=len;
            return res_len;
        }
        memcpy(str, gz->buf+gz->begin, last);
        gz->begin+=last;
        res_len+=last;
        str+=last;
        len-=last;
        if(gz->is_eof) return res_len;
    }
}

void gz_write(GzStream *gz, char *str, int len){
    while(1){
        int last=gz_stream_buff_size-gz->begin;
        if(len<=last){
            memcpy(gz->buf+gz->begin, str, len);
            gz->begin+=len;
            return;
        }
        //--
        memcpy(gz->buf+gz->begin, str, last);
        if(gz->fp) gzwrite(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
        else fwrite(gz->buf, 1, gz_stream_buff_size, gz->out);
        //--
        gz->begin=0;
        str+=last;
        len-=last;
    }
}

void gz_write_char(GzStream *gz, char c){
    if(gz->begin==gz_stream_buff_size){
        if(gz->fp) gzwrite(gz->fp, gz->buf, gz_stream_buff_size);
#ifdef _use_bz2
            else if(gz->bz2File) BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz_stream_buff_size);
#endif // _use_bz2
        else fwrite(gz->buf, 1, gz_stream_buff_size, gz->out);
        gz->begin=0;
    }
    gz->buf[gz->begin++]=c;
}

void gz_stream_destory(GzStream *gz){
    if(gz->is_write){
        if(gz->fp){
            gzwrite(gz->fp, gz->buf, gz->begin);
            gzclose(gz->fp);
        }
#ifdef _use_bz2
            else if(gz->bz2File){
            BZ2_bzWrite(&(gz->bzerror), gz->bz2File, gz->buf, gz->begin);
            BZ2_bzWriteClose(&(gz->bzerror), gz->bz2File, 0, NULL, NULL);
        }
#endif // _use_bz2
        else{
            fwrite(gz->buf, 1, gz->begin, gz->out);
            fclose(gz->out);
        }
    }else{
        if(gz->fp) gzclose(gz->fp);
#ifdef _use_bz2
        else BZ2_bzReadClose(&(gz->bzerror), gz->bz2File);
#endif // _use_bz2
    }
    if(gz->buf) free(gz->buf);
    if(gz->filename) free(gz->filename);
    free(gz);
}

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac)\
          {\
          a += F(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define GG(a,b,c,d,x,s,ac)\
          {\
          a += G(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define HH(a,b,c,d,x,s,ac)\
          {\
          a += H(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }
#define II(a,b,c,d,x,s,ac)\
          {\
          a += I(b,c,d) + x + ac;\
          a = ROTATE_LEFT(a,s);\
          a += b;\
          }

typedef struct{
    unsigned int count[2];
    unsigned int state[4];
    unsigned char buffer[64];
}MD5_CTX;

void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen);
void MD5Final(MD5_CTX *context,unsigned char digest[16], unsigned char *PADDING);
void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len);
void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len);
void MD5Transform(unsigned int state[4],unsigned char block[64]);

inline void MD5Init(MD5_CTX *context) {context->count[0] = 0;context->count[1] = 0;context->state[0] = 0x67452301;context->state[1] = 0xEFCDAB89;context->state[2] = 0x98BADCFE;context->state[3] = 0x10325476;}

void MD5Update(MD5_CTX *context,unsigned char *input,unsigned int inputlen) {
    unsigned int i = 0,index = 0,partlen = 0;
    index = (context->count[0] >> 3) & 0x3F;
    partlen = 64 - index;
    context->count[0] += inputlen << 3;
    if(context->count[0] < (inputlen << 3))
        context->count[1]++;
    context->count[1] += inputlen >> 29;

    if(inputlen >= partlen) {
        memcpy(&context->buffer[index],input,partlen);
        MD5Transform(context->state,context->buffer);
        for(i = partlen;i+64 <= inputlen;i+=64)
            MD5Transform(context->state,&input[i]);
        index = 0;
    }
    else {
        i = 0;
    }
    memcpy(&context->buffer[index],&input[i],inputlen-i);
}

inline void MD5Final(MD5_CTX *context,unsigned char digest[16], unsigned char *PADDING) {unsigned int index = 0,padlen = 0;unsigned char bits[8];index = (context->count[0] >> 3) & 0x3F;padlen = (index < 56)?(56-index):(120-index);MD5Encode(bits,context->count,8);MD5Update(context,PADDING,padlen);MD5Update(context,bits,8);MD5Encode(digest,context->state,16);}

void MD5Encode(unsigned char *output,unsigned int *input,unsigned int len) {
    unsigned int i = 0,j = 0;
    while(j < len) {
        output[j] = input[i] & 0xFF;
        output[j+1] = (input[i] >> 8) & 0xFF;
        output[j+2] = (input[i] >> 16) & 0xFF;
        output[j+3] = (input[i] >> 24) & 0xFF;
        i++;
        j+=4;
    }
}

void MD5Decode(unsigned int *output,unsigned char *input,unsigned int len) {
    unsigned int i = 0,j = 0;
    while(j < len) {
        output[i] = (input[j]) |
                    (input[j+1] << 8) |
                    (input[j+2] << 16) |
                    (input[j+3] << 24);
        i++;
        j+=4;
    }
}

void MD5Transform(unsigned int state[4],unsigned char block[64]) {
    unsigned int a = state[0];
    unsigned int b = state[1];
    unsigned int c = state[2];
    unsigned int d = state[3];
    unsigned int x[64];
    MD5Decode(x,block,64);
    FF(a, b, c, d, x[ 0], 7, 0xd76aa478); /* 1 */
    FF(d, a, b, c, x[ 1], 12, 0xe8c7b756); /* 2 */
    FF(c, d, a, b, x[ 2], 17, 0x242070db); /* 3 */
    FF(b, c, d, a, x[ 3], 22, 0xc1bdceee); /* 4 */
    FF(a, b, c, d, x[ 4], 7, 0xf57c0faf); /* 5 */
    FF(d, a, b, c, x[ 5], 12, 0x4787c62a); /* 6 */
    FF(c, d, a, b, x[ 6], 17, 0xa8304613); /* 7 */
    FF(b, c, d, a, x[ 7], 22, 0xfd469501); /* 8 */
    FF(a, b, c, d, x[ 8], 7, 0x698098d8); /* 9 */
    FF(d, a, b, c, x[ 9], 12, 0x8b44f7af); /* 10 */
    FF(c, d, a, b, x[10], 17, 0xffff5bb1); /* 11 */
    FF(b, c, d, a, x[11], 22, 0x895cd7be); /* 12 */
    FF(a, b, c, d, x[12], 7, 0x6b901122); /* 13 */
    FF(d, a, b, c, x[13], 12, 0xfd987193); /* 14 */
    FF(c, d, a, b, x[14], 17, 0xa679438e); /* 15 */
    FF(b, c, d, a, x[15], 22, 0x49b40821); /* 16 */

    /* Round 2 */
    GG(a, b, c, d, x[ 1], 5, 0xf61e2562); /* 17 */
    GG(d, a, b, c, x[ 6], 9, 0xc040b340); /* 18 */
    GG(c, d, a, b, x[11], 14, 0x265e5a51); /* 19 */
    GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa); /* 20 */
    GG(a, b, c, d, x[ 5], 5, 0xd62f105d); /* 21 */
    GG(d, a, b, c, x[10], 9,  0x2441453); /* 22 */
    GG(c, d, a, b, x[15], 14, 0xd8a1e681); /* 23 */
    GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8); /* 24 */
    GG(a, b, c, d, x[ 9], 5, 0x21e1cde6); /* 25 */
    GG(d, a, b, c, x[14], 9, 0xc33707d6); /* 26 */
    GG(c, d, a, b, x[ 3], 14, 0xf4d50d87); /* 27 */
    GG(b, c, d, a, x[ 8], 20, 0x455a14ed); /* 28 */
    GG(a, b, c, d, x[13], 5, 0xa9e3e905); /* 29 */
    GG(d, a, b, c, x[ 2], 9, 0xfcefa3f8); /* 30 */
    GG(c, d, a, b, x[ 7], 14, 0x676f02d9); /* 31 */
    GG(b, c, d, a, x[12], 20, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH(a, b, c, d, x[ 5], 4, 0xfffa3942); /* 33 */
    HH(d, a, b, c, x[ 8], 11, 0x8771f681); /* 34 */
    HH(c, d, a, b, x[11], 16, 0x6d9d6122); /* 35 */
    HH(b, c, d, a, x[14], 23, 0xfde5380c); /* 36 */
    HH(a, b, c, d, x[ 1], 4, 0xa4beea44); /* 37 */
    HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9); /* 38 */
    HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60); /* 39 */
    HH(b, c, d, a, x[10], 23, 0xbebfbc70); /* 40 */
    HH(a, b, c, d, x[13], 4, 0x289b7ec6); /* 41 */
    HH(d, a, b, c, x[ 0], 11, 0xeaa127fa); /* 42 */
    HH(c, d, a, b, x[ 3], 16, 0xd4ef3085); /* 43 */
    HH(b, c, d, a, x[ 6], 23,  0x4881d05); /* 44 */
    HH(a, b, c, d, x[ 9], 4, 0xd9d4d039); /* 45 */
    HH(d, a, b, c, x[12], 11, 0xe6db99e5); /* 46 */
    HH(c, d, a, b, x[15], 16, 0x1fa27cf8); /* 47 */
    HH(b, c, d, a, x[ 2], 23, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II(a, b, c, d, x[ 0], 6, 0xf4292244); /* 49 */
    II(d, a, b, c, x[ 7], 10, 0x432aff97); /* 50 */
    II(c, d, a, b, x[14], 15, 0xab9423a7); /* 51 */
    II(b, c, d, a, x[ 5], 21, 0xfc93a039); /* 52 */
    II(a, b, c, d, x[12], 6, 0x655b59c3); /* 53 */
    II(d, a, b, c, x[ 3], 10, 0x8f0ccc92); /* 54 */
    II(c, d, a, b, x[10], 15, 0xffeff47d); /* 55 */
    II(b, c, d, a, x[ 1], 21, 0x85845dd1); /* 56 */
    II(a, b, c, d, x[ 8], 6, 0x6fa87e4f); /* 57 */
    II(d, a, b, c, x[15], 10, 0xfe2ce6e0); /* 58 */
    II(c, d, a, b, x[ 6], 15, 0xa3014314); /* 59 */
    II(b, c, d, a, x[13], 21, 0x4e0811a1); /* 60 */
    II(a, b, c, d, x[ 4], 6, 0xf7537e82); /* 61 */
    II(d, a, b, c, x[11], 10, 0xbd3af235); /* 62 */
    II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb); /* 63 */
    II(b, c, d, a, x[ 9], 21, 0xeb86d391); /* 64 */
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

char *get_md5(char *str, int str_len, int is_file){
    unsigned char PADDING[]={
            0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };

    MD5_CTX md5;
    MD5Init(&md5);

    if(is_file){
        int64_t size=1024*1024;
        char buff[1024*1024];

        FILE *f=fopen(str, "r");
        if(f==NULL){
            fprintf(stderr, "open file(%s) error, please check!\n", str);
            exit(0);
        }

        fseek(f, 0, SEEK_END);
        int64_t file_size=ftell(f);
        rewind(f);

        int64_t i, num=file_size/size;
        for(i=0;i<num;i++){
            if(fread(buff, 1, size, f)!=size){
                fprintf(stderr, "Error in read file(%s), in calculate_MD5\n", str);
                exit(0);
            }
            MD5Update(&md5, buff, size);
        }
        int64_t last=file_size-num*size;
        if(last>0){
            if(fread(buff, 1, last, f)!=last){
                fprintf(stderr, "Error in read file(%s), in calculate_MD5\n", str);
                exit(0);
            }
            MD5Update(&md5, buff, last);
        }

        fclose(f);
    }else{
        MD5Update(&md5, str, str_len);
    }

    unsigned char decrypt[16];
    MD5Final(&md5, decrypt, PADDING);

    char *res=my_new(33, sizeof(char));
    int i;
    for(i=0;i<16;i++) {
        sprintf(res+i*2, "%02x", decrypt[i]);
    }

    return res;
}

//================ bio

char *get_fasta_name(char *str, int len){
    int i;
    char c;
    for(i=0;i<len;i++){
        c=str[i];
        if(c==' ' || c=='\t') break;
    }
    char *res=my_new(i, sizeof(char));
    memcpy(res, str+1, (i-1)*sizeof(char));
    return res;
}

inline char get_anti_base(char c){return anti_base_table[c];}

char *anti_rev_seq(char *seq, int len){
    char *res=my_new(len+1, sizeof(char));
    int i=0, j=len-1;
    for(;i<len;i++,j--) res[i]=anti_base_table[seq[j]];
    return res;
}

//================ sam bam

inline int is_big_endian(){long one= 1;return !(*((char *)(&one)));}

inline uint16_t swap_endian_2(uint16_t v){return (uint16_t)(((v & 0x00FF00FFU) << 8) | ((v & 0xFF00FF00U) >> 8));}

inline void *swap_endian_2p(void *x){*(uint16_t*)x = swap_endian_2(*(uint16_t*)x);return x;}

inline uint32_t swap_endian_4(uint32_t v){v = ((v & 0x0000FFFFU) << 16) | (v >> 16);return ((v & 0x00FF00FFU) << 8) | ((v & 0xFF00FF00U) >> 8);}

inline void *swap_endian_4p(void *x){*(uint32_t*)x = swap_endian_4(*(uint32_t*)x);return x;}

inline uint64_t swap_endian_8(uint64_t v){v = ((v & 0x00000000FFFFFFFFLLU) << 32) | (v >> 32);v = ((v & 0x0000FFFF0000FFFFLLU) << 16) | ((v & 0xFFFF0000FFFF0000LLU) >> 16);return ((v & 0x00FF00FF00FF00FFLLU) << 8) | ((v & 0xFF00FF00FF00FF00LLU) >> 8);}

inline void *swap_endian_8p(void *x){*(uint64_t*)x = swap_endian_8(*(uint64_t*)x);return x;}

int to_bam_header(int l_chrNames, char **chrNames, int *chrLengths, char *ID, char *SM, char *LB, char *buff){
    int i, is_big=is_big_endian();

    char *original=buff;
    buff[0]='B';
    buff[1]='A';
    buff[2]='M';
    buff[3]=1;
    buff+=8;

    for(i=0;i<l_chrNames;i++){
        buff+=sprintf(buff, "@SQ\tSN:%s\tLN:%d\n", chrNames[i], chrLengths[i]);
    }
    buff+=sprintf(buff, "@RG\tID:%s\tSM:%s\tLB:%s\tPL:ILLUMINA\tPU:run\n", ID, SM, LB);

    {
        int32_t *ptr_original=(int32_t *)(original+4);
        *(ptr_original)=is_big ? swap_endian_4(buff-original-8) : (buff-original-8);
        int32_t *ptr_buff=(int32_t *)buff;
        *(ptr_buff)=is_big ? swap_endian_4(l_chrNames) : l_chrNames;
        buff+=4;
    }

    for(i=0;i<l_chrNames;i++){
        char *name=chrNames[i];
        int len=chrLengths[i];
        {
            int32_t *ptr_buff=(int32_t *)buff;
            *(ptr_buff)=is_big ? swap_endian_4(strlen(name)+1) : (strlen(name)+1);
            buff+=4;
        }

        buff+=sprintf(buff, "%s", name);
        buff++;

        {
            int32_t *ptr_buff=(int32_t *)buff;
            *(ptr_buff)=is_big ? swap_endian_4(len) : len;
            buff+=4;
        }
    }

    return buff-original;
}

void skip_bam_header(GzStream *bam, char *buff){
    int32_t i, datasize, n_targets, name_len, target_len, is_big=is_big_endian();
    gz_read(bam, buff, 4);
    gz_read(bam, &datasize, 4);if(is_big) datasize= swap_endian_4(datasize);
    gz_read(bam, buff, datasize);
    gz_read(bam, &n_targets, 4);if(is_big) n_targets= swap_endian_4(n_targets);
    for(i=0;i<n_targets;i++){
        gz_read(bam, &name_len, 4);if(is_big) name_len= swap_endian_4(name_len);
        gz_read(bam, buff, name_len);
        gz_read(bam, &target_len, 4);if(is_big) target_len= swap_endian_4(target_len);
    }
}

BamHeader *read_bam_header(GzStream *bam, char *buff){
    BamHeader *header=calloc(1, sizeof(BamHeader));

    int32_t i, datasize, n_targets, name_len, target_len, is_big=is_big_endian();
    gz_read(bam, buff, 4);
    gz_read(bam, &datasize, 4);if(is_big) datasize= swap_endian_4(datasize);
    gz_read(bam, buff, datasize);
    header->header=str_copy_with_len(buff, datasize);
    header->l_header=datasize;
    gz_read(bam, &n_targets, 4);if(is_big) n_targets= swap_endian_4(n_targets);
    header->l_chr=n_targets;
    header->chr_names=calloc(n_targets, sizeof(char *));
    header->l_chr_names=calloc(n_targets, sizeof(int));
    header->chr_lens=calloc(n_targets, sizeof(int));
    for(i=0;i<n_targets;i++){
        gz_read(bam, &name_len, 4);if(is_big) name_len= swap_endian_4(name_len);
        gz_read(bam, buff, name_len);
        header->chr_names[i]=str_copy_with_len(buff, name_len-1);
        header->l_chr_names[i]=name_len-1;
        gz_read(bam, &target_len, 4);if(is_big) target_len= swap_endian_4(target_len);
        header->chr_lens[i]=target_len;
    }

    return header;
}

void free_bamHeader(BamHeader *header){
    int i;
    for(i=0;i<header->l_chr;i++) free(header->chr_names[i]);
    free(header->chr_names);
    free(header->l_chr_names);
    free(header->chr_lens);
    free(header->header);
    free(header);
}

int read_bam_to_sam(GzStream *bam, int l_chrNames, char **chrNames, char *buff, char *sam){
    int32_t datasize;
    if(gz_read(bam, &datasize, 4)==0) return 0;
    if(is_big_endian()) datasize= swap_endian_4(datasize);
    gz_read(bam, buff, datasize);
    return bam_to_sam(l_chrNames, chrNames, datasize, buff, sam);
}

int bam_to_sam(int l_chrNames, char **chrNames, int datasize, char *buff, char *sam){
    char *original=buff;
    char *original_sam=sam;

    int32_t i, j, len, is_big=is_big_endian();

    uint32_t *x=(uint32_t *)buff;
    if(is_big){
        for(i=0;i<8;i++) x[i]= swap_endian_4(x[i]);
    }
    int tid=x[0];
    int pos=x[1];
    int bin=x[2]>>16;
    int qual=(0xFF00&x[2])>>8;
    int l_qname=0xFF&x[2];
    int flag=x[3]>>16;
    int n_cigar=0xFFFF&x[3];
    int l_qseq=x[4];
    int mtid=x[5];
    int mpos=x[6];
    int isize=x[7];
    buff+=32;

    //--name
    memcpy(sam, buff, l_qname-1);
    sam[l_qname-1]='\t';
    sam+=l_qname;
    buff+=l_qname;

    //--flag
    sam+=sprintf(sam, "%d\t", flag);

    //--chr
    if(tid==-1){sam[0]='*';sam[1]='\t';sam+=2;}
    else sam+=sprintf(sam, "%s\t", chrNames[tid]);

    //--pos
    if(pos==-1) sam+=sprintf(sam, "*\t");
    else sam+=sprintf(sam, "%d\t", pos+1);

    //--qual
    sam+=sprintf(sam, "%d\t", qual);

    //--cigar
    x=(int32_t *)buff;
    for(i=0;i<n_cigar;i++){
        if(is_big) x[i]= swap_endian_4(x[i]);
        int n=x[i]>>4;
        int o=x[i]&0xF;
        if(o==0) sam+=sprintf(sam, "%dM", n);
        else if(o==1) sam+=sprintf(sam, "%dI", n);
        else if(o==2) sam+=sprintf(sam, "%dD", n);
        else if(o==3) sam+=sprintf(sam, "%dN", n);
        else if(o==4) sam+=sprintf(sam, "%dS", n);
        else if(o==5) sam+=sprintf(sam, "%dH", n);
        else if(o==6) sam+=sprintf(sam, "%dP", n);
    }
    sam[0]='\t';
    sam++;
    buff+=n_cigar*4;

    //--mchr
    if(mtid==-1){sam[0]='*';sam[1]='\t';sam+=2;}
    else if(mtid==tid){sam[0]='=';sam[1]='\t';sam+=2;}
    else sam+=sprintf(sam, "%s\t", chrNames[mtid]);

    //--mpos
    if(mpos==-1) sam+=sprintf(sam, "*\t");
    else sam+=sprintf(sam, "%d\t", mpos+1);

    //--isize
    sam+=sprintf(sam, "%d\t", isize);

    //--seq
    uint8_t p=*(buff++);
    for(i=0;i<l_qseq;i++){
        *(sam++)=bam_int_2_base[(p>>(4*(1-i%2)))&0xF];
        if(i%2==1 && i!=(l_qseq-1)) p=*(buff++);
    }
    *(sam++)='\t';

    //--qual
    for(i=0;i<l_qseq;i++) *(sam++)=*(buff++)+33;
    *(sam++)='\t';

    while((buff-original)<datasize){
        *(sam++)=*(buff++);
        *(sam++)=*(buff++);
        *(sam++)=':';
        char type=*(buff++);
        if(type=='A'){
            *(sam++)=type;
            *(sam++)=':';
            *(sam++)=*(buff++);
        }else if(type=='c'){
            *(sam++)='i';
            *(sam++)=':';
            int8_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff++;
        }else if(type=='s'){
            *(sam++)='i';
            *(sam++)=':';
            int16_t *ptr=buff;if(is_big) swap_endian_2p(ptr);
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=2;
        }else if(type=='i'){
            *(sam++)='i';
            *(sam++)=':';
            int32_t *ptr=buff;if(is_big) swap_endian_4p(ptr);
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=4;
        }else if(type=='C'){
            *(sam++)='i';
            *(sam++)=':';
            uint8_t *ptr=buff;
            sam+=sprintf(sam, "%d", ptr[0]);
            buff++;
        }else if(type=='S'){
            *(sam++)='i';
            *(sam++)=':';
            uint16_t *ptr=buff;if(is_big) swap_endian_2p(ptr);
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=2;
        }else if(type=='I'){
            *(sam++)='i';
            *(sam++)=':';
            uint32_t *ptr=buff;if(is_big) swap_endian_4p(ptr);
            sam+=sprintf(sam, "%d", ptr[0]);
            buff+=4;
        }else if(type=='f'){
            *(sam++)=type;
            *(sam++)=':';
            float *ptr=buff;if(is_big) swap_endian_4p(ptr);
            sam+=sprintf(sam, "%f", ptr[0]);
            buff+=4;
        }else if(type=='d'){
            *(sam++)=type;
            *(sam++)=':';
            double *ptr=buff;if(is_big) swap_endian_8p(ptr);
            sam+=sprintf(sam, "%f", ptr[0]);
            buff+=8;
        }else if(type=='Z' || type=='H'){
            *(sam++)=type;
            *(sam++)=':';
            len=strlen(buff);
            memcpy(sam, buff, len);
            sam+=len;
            buff+=(len+1);
        }
        *(sam++)='\t';
    }
    sam--;
    sam[0]='\0';
    sam[1]='\0';

    return sam-original_sam;
}

int sam_to_bam(int l_chrNames, char **chrNames, char *sam, int *tabs, char *buff){
    int i, j, l_tabs=0, is_big=is_big_endian();

    char *original=buff;
    buff+=36;

    //-- calculate tabs positions
    for(i=0,j=0;;i++){
        char c=sam[i];
        if(c=='\t') tabs[j++]=i;
        else if(c=='\n' || c=='\0'){
            tabs[j++]=i;
            break;
        }
    }
    l_tabs=j;

    //-- unmapped, not convert
    //i=tabs[1]+1;
    //if(sam[i]=='*') return 0;

    //-- qual
    i=tabs[3]+1;
    j=tabs[4];
    sam[j]='\0';
    int qual=isdigit(sam[i])? atoi(sam+i) : 0;
    sam[j]='\t';

    //-- name
    i=0;
    j=tabs[0];
    sam[j]='\0';
    int l_qname=j-i+1;
    memcpy(buff, sam+i, (l_qname)*sizeof(char));
    buff+=l_qname;
    sam[j]='\t';

    //-- flag
    i=tabs[0]+1;
    j=tabs[1];
    sam[j]='\0';
    int flag=atoi(sam+i);
    sam[j]='\t';

    //-- tid
    int tid=-1;
    if(chrNames==NULL) tid=0;
    else{
        i=tabs[1]+1;
        j=tabs[2];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){tid=i;break;}}
        sam[j]='\t';
        //if(tid==-1){fprintf(stderr, "Error tid: %s\n", sam);exit(0);}
    }

    //-- pos
    i=tabs[2]+1;
    j=tabs[3];
    sam[j]='\0';
    int pos=isdigit(sam[i]) ? atoi(sam+i)-1 : -1;
    sam[j]='\t';

    //-- cigar
    i=tabs[4]+1;
    j=tabs[5];
    sam[j]='\0';
    int bin=0, n_cigar=0;
    int32_t *cigar=NULL;
    {
        char *s, *t, *str=sam+i;
        int op;
        int64_t x;
        if(str[0]!='*'){
            for(s=str;*s;++s){if(isalpha(*s)) ++n_cigar;}
            cigar=(int32_t *)my_new(n_cigar, sizeof(int32_t));
            for(s=str,i=0;i<n_cigar;++i){
                x=strtol(s, &t, 10);
                op=toupper(*t);
                s=t+1;
                if(op=='M') op = 0;
                else if(op=='I') op = 1;
                else if(op=='D') op = 2;
                else if(op=='N') op = 3;
                else if(op=='S') op = 4;
                else if(op=='H') op = 5;
                else if(op=='P') op = 6;
                cigar[i]=(int32_t)(x<<4|op);
                int32_t *ptr_buff=(int32_t *)buff;
                *(ptr_buff)=is_big ? swap_endian_4(cigar[i]) : cigar[i];
                buff+=4;
            }
            bin=bam_reg2bin(pos, bam_calend(pos, n_cigar, cigar));
        }else bin=bam_reg2bin(pos, pos+1);
    }
    sam[j]='\t';

    //-- mtid
    i=tabs[5]+1;
    int mtid=-1;
    if(sam[i]=='=') mtid=tid;
    else if(sam[i]=='*' || chrNames==NULL) mtid=-1;
    else{
        j=tabs[6];
        sam[j]='\0';
        char *str=sam+i;
        for(i=0;i<l_chrNames;i++){if(strcmp(str, chrNames[i])==0){mtid=i;break;}}
        sam[j]='\t';
        //if(mtid==-1){fprintf(stderr, "Error mtid: %s\n", sam);exit(0);}
    }

    //-- mpos
    i=tabs[6]+1;
    j=tabs[7];
    sam[j]='\0';
    int mpos=isdigit(sam[i]) ? atoi(sam+i)-1:-1;
    sam[j]='\t';

    //-- isize
    i=tabs[7]+1;
    j=tabs[8];
    sam[j]='\0';
    int isize=(sam[i]=='-' || isdigit(sam[i])) ? atoi(sam+i):0;
    sam[j]='\t';

    //-- seq
    int l_qseq=0;
    {
        uint8_t p;

        i=tabs[8]+1;
        j=tabs[9];
        sam[j]='\0';
        char *str=sam+i;
        l_qseq=j-i;
        for(i=0;i<l_qseq;++i){
            if((i%2)==0) p=0;
            p|=bam_nt16_table[(int)str[i]] << 4*(1-i%2);
            if((i%2)==1){*(buff)=p;buff++;}
        }
        if((l_qseq%2)==1){*(buff)=p;buff++;}
        sam[j]='\t';

        i=tabs[9]+1;
        j=tabs[10];
        sam[j]='\0';
        str=sam+i;
        if((j-i)==1 && str[0]=='*'){    //strcmp(str, "*")==0
            for(i=0;i<l_qseq;++i){*(buff)=0xff;buff++;}
        }else{
            for(i=0;i<l_qseq;++i){*(buff)=str[i]-33;buff++;}
        }
        sam[j]='\t';
    }

    //--
    char pre;
    int index, max=l_tabs-1;
    for(index=10;index<max;index++,sam[j]=pre){
        i=tabs[index]+1;
        j=tabs[index+1];
        pre=sam[j];
        sam[j]='\0';
        char *str=sam+i;
        //--
        *(buff)=str[0];buff++;
        *(buff)=str[1];buff++;
        char type=str[3];
        if(type=='A' || type=='a' || type=='c' || type=='C'){ // c and C for backward compatibility
            *(buff)='A';buff++;
            *(buff)=str[5];buff++;
        }else if(type=='I' || type=='i'){
            int64_t x=atoll(str+5);
            if(x<0){
                if(x>=-127){
                    *(buff)='c';buff++;
                    int8_t *ptr_buff=(int8_t *)buff;
                    *(ptr_buff)=(int8_t)x;
                    buff++;
                }else if(x>=-32767){
                    *(buff)='s';buff++;
                    int16_t *ptr_buff=(int16_t *)buff;
                    *(ptr_buff)=(int16_t)x;if(is_big) swap_endian_2p(ptr_buff);
                    buff+=2;
                }else{
                    *(buff)='i';buff++;
                    int32_t *ptr_buff=(int32_t *)buff;
                    *(ptr_buff)=(int32_t)x;if(is_big) swap_endian_4p(ptr_buff);
                    buff+=4;
                }
            }else{
                if(x<=255){
                    *(buff)='C';buff++;
                    uint8_t *ptr_buff=(uint8_t *)buff;
                    *(ptr_buff)=(uint8_t)x;
                    buff++;
                }else if(x<=65535){
                    *(buff)='S';buff++;
                    uint16_t *ptr_buff=(uint16_t *)buff;
                    *(ptr_buff)=(uint16_t)x;if(is_big) swap_endian_2p(ptr_buff);
                    buff+=2;
                }else{
                    *(buff)='I';buff++;
                    uint32_t *ptr_buff=(uint32_t *)buff;
                    *(ptr_buff)=(uint32_t)x;if(is_big) swap_endian_4p(ptr_buff);
                    buff+=4;
                }
            }
        }else if(type=='f'){
            *(buff)=type;buff++;
            float *ptr_buff=(float *)buff;
            *(ptr_buff)=atof(str+5);if(is_big) swap_endian_4p(ptr_buff);
            buff+=4;
        }else if(type=='d'){
            *(buff)=type;buff++;
            double *ptr_buff=(double *)buff;
            *(ptr_buff)=atof(str+9);if(is_big) swap_endian_8p(ptr_buff);
            buff+=8;
        }else if(type=='Z' || type=='H'){
            *(buff)=type;buff++;
            int len=j-i-5+1;
            memcpy(buff, str+5, len*sizeof(char));
            buff+=len;
        }
    }

    //--
    uint32_t x[8];
    x[0] = tid;
    x[1] = pos;
    x[2] = (uint32_t)bin<<16 | qual<<8 | l_qname;
    x[3] = (uint32_t)flag<<16 | n_cigar;
    x[4] = l_qseq;
    x[5] = mtid;
    x[6] = mpos;
    x[7] = isize;

    int len=buff-original;
    int32_t block_len=len-4;

    uint32_t *ptr_original=(uint32_t *)original;
    ptr_original[0]=is_big ? swap_endian_4(block_len) : block_len;
    for(i=0;i<8;i++) ptr_original[i+1]=is_big ? swap_endian_4(x[i]) : x[i];

    if(cigar) free(cigar);
    return len;
}

uint32_t bam_calend(int pos, int n_cigar, int32_t *cigar){
    uint32_t k, end;
    end = pos;
    for (k=0; k<n_cigar; ++k){
        int op = cigar[k] & 15;
        if (op == 0 || op == 2 || op == 3)
            end += cigar[k] >> 4;
    }
    return end;
}

inline int bam_reg2bin(uint32_t beg, uint32_t end){--end;if (beg>>14 == end>>14) return 4681 + (beg>>14);if (beg>>17 == end>>17) return  585 + (beg>>17);if (beg>>20 == end>>20) return   73 + (beg>>20);if (beg>>23 == end>>23) return    9 + (beg>>23);if (beg>>26 == end>>26) return    1 + (beg>>26);return 0;}

//==================== java1.6 sort

inline void java_sort_int(int *arr, int left, int length){java_sort_int_inner(arr, left, length);}

static void java_sort_int_inner(int *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
        for (i=off; i<len+off; i++)
            for (j=i; j>off && x[j-1]>x[j]; j--)
                java_sort_int_swap(x, j, j-1);
        return;
    }

    // Choose a partition element, v
    int m = off + (len >> 1);       // Small arrays, middle element
    if (len > 7) {
        int l = off;
        int n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudomedian of 9
            int s = len/8;
            l = java_sort_int_med3(x, l,     l+s, l+2*s);
            m = java_sort_int_med3(x, m-s,   m,   m+s);
            n = java_sort_int_med3(x, n-2*s, n-s, n);
        }
        m = java_sort_int_med3(x, l, m, n); // Mid-size, med of 3
    }
    int v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = a, c = off + len - 1, d = c;
    while(true) {
        while (b <= c && x[b] <= v) {
            if (x[b] == v)
                java_sort_int_swap(x, a++, b);
            b++;
        }
        while (c >= b && x[c] >= v) {
            if (x[c] == v)
                java_sort_int_swap(x, c, d--);
            c--;
        }
        if (b > c)
            break;
        java_sort_int_swap(x, b++, c--);
    }

    // Swap partition elements back to middle
    int s, n = off + len;

    int tmp1=a-off;
    int tmp2=b-a;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_int_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_int_vecswap(x, b,   n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1)
        java_sort_int_inner(x, off, s);
    if ((s = d-c) > 1)
        java_sort_int_inner(x, n-s, s);
}

static inline void java_sort_int_swap(int *x, int a, int b){int t = x[a];x[a] = x[b];x[b] = t;}

static inline int java_sort_int_med3(int *x, int a, int b, int c){return (x[a] < x[b] ? (x[b] < x[c] ? b : x[a] < x[c] ? c : a) :(x[b] > x[c] ? b : x[a] > x[c] ? c : a));}

static inline void java_sort_int_vecswap(int *x, int a, int b, int n){int i;for (i=0; i<n; i++, a++, b++) java_sort_int_swap(x, a, b);}

inline void java_sort_long(int64_t *arr, int left, int length){java_sort_long_inner(arr, left, length);}

static void java_sort_long_inner(int64_t *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
        for (i=off; i<len+off; i++)
            for (j=i; j>off && x[j-1]>x[j]; j--)
                java_sort_long_swap(x, j, j-1);
        return;
    }

    // Choose a partition element, v
    int m = off + (len >> 1);       // Small arrays, middle element
    if (len > 7) {
        int l = off;
        int n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudomedian of 9
            int s = len/8;
            l = java_sort_long_med3(x, l,     l+s, l+2*s);
            m = java_sort_long_med3(x, m-s,   m,   m+s);
            n = java_sort_long_med3(x, n-2*s, n-s, n);
        }
        m = java_sort_long_med3(x, l, m, n); // Mid-size, med of 3
    }
    int64_t v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = a, c = off + len - 1, d = c;
    while(true) {
        while (b <= c && x[b] <= v) {
            if (x[b] == v)
                java_sort_long_swap(x, a++, b);
            b++;
        }
        while (c >= b && x[c] >= v) {
            if (x[c] == v)
                java_sort_long_swap(x, c, d--);
            c--;
        }
        if (b > c)
            break;
        java_sort_long_swap(x, b++, c--);
    }

    // Swap partition elements back to middle
    int s, n = off + len;

    int tmp1=a-off;
    int tmp2=b-a;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_long_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_long_vecswap(x, b,   n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1)
        java_sort_long_inner(x, off, s);
    if ((s = d-c) > 1)
        java_sort_long_inner(x, n-s, s);
}

static inline void java_sort_long_swap(int64_t *x, int a, int b){int64_t t = x[a];x[a] = x[b];x[b] = t;}

static inline int java_sort_long_med3(int64_t *x, int a, int b, int c){return (x[a] < x[b] ?(x[b] < x[c] ? b : x[a] < x[c] ? c : a) :(x[b] > x[c] ? b : x[a] > x[c] ? c : a));}

static inline void java_sort_long_vecswap(int64_t *x, int a, int b, int n){int i;for (i=0; i<n; i++, a++, b++) java_sort_long_swap(x, a, b);}

inline void java_sort_double(double *arr, int left, int length){java_sort_double_inner(arr, left, length);}

static void java_sort_double_inner(double *x, int off, int len){
    int i, j;

    // Insertion sort on smallest arrays
    if (len < 7) {
        for (i=off; i<len+off; i++)
            for (j=i; j>off && x[j-1]>x[j]; j--)
                java_sort_double_swap(x, j, j-1);
        return;
    }

    // Choose a partition element, v
    int m = off + (len >> 1);       // Small arrays, middle element
    if (len > 7) {
        int l = off;
        int n = off + len - 1;
        if (len > 40) {        // Big arrays, pseudomedian of 9
            int s = len/8;
            l = java_sort_double_med3(x, l,     l+s, l+2*s);
            m = java_sort_double_med3(x, m-s,   m,   m+s);
            n = java_sort_double_med3(x, n-2*s, n-s, n);
        }
        m = java_sort_double_med3(x, l, m, n); // Mid-size, med of 3
    }
    double v = x[m];

    // Establish Invariant: v* (<v)* (>v)* v*
    int a = off, b = a, c = off + len - 1, d = c;
    while(true) {
        while (b <= c && x[b] <= v) {
            if (x[b] == v)
                java_sort_double_swap(x, a++, b);
            b++;
        }
        while (c >= b && x[c] >= v) {
            if (x[c] == v)
                java_sort_double_swap(x, c, d--);
            c--;
        }
        if (b > c)
            break;
        java_sort_double_swap(x, b++, c--);
    }

    // Swap partition elements back to middle
    int s, n = off + len;

    int tmp1=a-off;
    int tmp2=b-a;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_double_vecswap(x, off, b-s, s);

    tmp1=d-c;
    tmp2=n-d-1;
    s=tmp1<tmp2 ? tmp1:tmp2;
    java_sort_double_vecswap(x, b,   n-s, s);

    // Recursively sort non-partition-elements
    if ((s = b-a) > 1)
        java_sort_double_inner(x, off, s);
    if ((s = d-c) > 1)
        java_sort_double_inner(x, n-s, s);
}

static inline void java_sort_double_swap(double *x, int a, int b){double t = x[a];x[a] = x[b];x[b] = t;}

static inline int java_sort_double_med3(double *x, int a, int b, int c){return (x[a] < x[b] ?(x[b] < x[c] ? b : x[a] < x[c] ? c : a) :(x[b] > x[c] ? b : x[a] > x[c] ? c : a));}

static inline void java_sort_double_vecswap(double *x, int a, int b, int n){int i;for (i=0; i<n; i++, a++, b++)java_sort_double_swap(x, a, b);}

void java_sort_void(void **arr, int left, int length, int(*mycompare)(void *a, void *b)){
    void **aux=(void **)my_new2(length*sizeof(void *));
    int i;
    for(i=0;i<length;i++){
        aux[i]=arr[left+i];
    }

    java_sort_void_merge_sort(aux, arr, left, left+length, -left, mycompare);

    free(aux);
}

void java_sort_void2(int64_t *arr, int left, int length, int(*mycompare)(void *a, void *b)){
    void **aux1=(void **)my_new2(length*sizeof(void *));
    void **aux2=(void **)my_new2(length*sizeof(void *));

    int i;
    for(i=0;i<length;i++){
        aux1[i]=(void *)arr[left+i];
        aux2[i]=(void *)arr[left+i];
    }

    java_sort_void_merge_sort(aux1, aux2, 0, length, 0, mycompare);

    for(i=0;i<length;i++){
        arr[left+i]=(int64_t)aux2[i];
    }

    free(aux1);
    free(aux2);
}

static void java_sort_void_merge_sort(void **src, void **dest, int low, int high, int off, int(*c)(void *a, void *b)){
    int length=high-low, i, j, p, q;

    if (length < 7) {
        for (i=low; i<high; i++)
            for (j=i; j>low && c(dest[j-1], dest[j])>0; j--)
                java_sort_void_swap(dest, j, j-1);
        return;
    }

    // Recursively sort halves of dest into src
    int destLow  = low;
    int destHigh = high;
    low  += off;
    high += off;
    int mid = (unsigned)(low + high) >> 1;
    java_sort_void_merge_sort(dest, src, low, mid, -off, c);
    java_sort_void_merge_sort(dest, src, mid, high, -off, c);

    // If list is already sorted, just copy from src to dest.  This is an
    // optimization that results in faster sorts for nearly ordered lists.
    if (c(src[mid-1], src[mid]) <= 0) {
        for(i=0;i<length;i++){
            dest[destLow+i]=src[low+i];
        }
        return;
    }

    // Merge sorted halves (now in src) into dest
    for(i = destLow, p = low, q = mid; i < destHigh; i++) {
        if (q >= high || p < mid && c(src[p], src[q]) <= 0)
            dest[i] = src[p++];
        else
            dest[i] = src[q++];
    }
}

static inline void java_sort_void_swap(void **x, int a, int b){void *t=x[a];x[a]=x[b];x[b]=t;}

void java_shuffle(void **arr, int left, int length, MyRand *rand){
    if(left>0){
        arr+=left;
        left=0;
    }

    int i;
    for(i=length;i>1;i--){
        int a=i-1;
        int b= my_rand_next_int(rand, i);
        if(a!=b){
            void *tmp=arr[a];
            arr[a]=arr[b];
            arr[b]=tmp;
        }
    }
}

//=============== java random

inline MyRand* new_my_rand1() {MyRand* mr = calloc(1, sizeof(MyRand));int64_t seed = clock();mr->seed = (seed ^ (int64_t)0x5DEECE66D) & (((int64_t)1 << 48) - 1);return mr;}

inline MyRand* new_my_rand2(int64_t seed) {MyRand* mr = calloc(1, sizeof(MyRand));mr->seed = (seed ^ (int64_t)0x5DEECE66D) & (((int64_t)1 << 48) - 1);return mr;}

static int my_rand_next(MyRand *mr, int bits) {
    int64_t oldseed, nextseed;;
    while (1) {
        oldseed = mr->seed;
        nextseed = (oldseed * (int64_t)0x5DEECE66D + (int64_t)0xB) & (((int64_t)1 << 48) - 1);
        if (oldseed != nextseed) {
            mr->seed = nextseed;
            break;
        }
    }
    return (int)((uint64_t)nextseed >> (48 - bits));
}

int my_rand_next_int(MyRand *mr, int bound) {
    int r = my_rand_next(mr, 31);
    int m = bound - 1;
    if ((bound & m) == 0)  // i.e., bound is a power of 2
        r = (int)((bound * (int64_t)r) >> 31);
    else {
        int u;
        for (u = r;
             u - (r = u % bound) + m < 0;
             u = my_rand_next(mr, 31))
            ;
    }
    return r;
}

inline double my_rand_next_double(MyRand *mr) {
    int n1 = my_rand_next(mr, 26);
    int n2 = my_rand_next(mr, 27);
    return (((int64_t)(n1) << 27) + n2) * 0x1.0p-53;
}

double my_rand_next_gaussian(MyRand* mr) {
    if (mr->haveNextNextGaussian) {
        mr->haveNextNextGaussian = 0;
        return mr->nextNextGaussian;
    }else {
        double v1, v2, s;
        do {
            v1 = 2.0 * my_rand_next_double(mr) - 1.0; // between -1 and 1
            v2 = 2.0 * my_rand_next_double(mr) - 1.0; // between -1 and 1
            s = v1 * v1 + v2 * v2;
        } while (s >= 1 || s == 0);
        double multiplier = sqrt(-2.0 * log(s) / s);
        mr->nextNextGaussian = v2 * multiplier;
        mr->haveNextNextGaussian = 1;
        return v1 * multiplier;
    }
}

//==================== math

inline int *new_i_matrix2(int64_t row, int64_t col){return my_new(row*col, sizeof(int));}

inline void set_i_matrix2(int *matrix, int64_t row, int64_t col, int64_t i, int64_t j, int value){matrix[i*col+j]=value;}

inline int get_i_matrix2(int *matrix, int64_t row, int64_t col, int64_t i, int64_t j){return matrix[i*col+j];}

inline int64_t *new_l_matrix2(int64_t row, int64_t col){return my_new(row*col, sizeof(int64_t));}

inline void set_l_matrix2(int64_t *matrix, int64_t row, int64_t col, int64_t i, int64_t j, int64_t value){matrix[i*col+j]=value;}

inline int64_t get_l_matrix2(int64_t *matrix, int64_t row, int64_t col, int64_t i, int64_t j){return matrix[i*col+j];}

inline float *new_f_matrix2(int64_t row, int64_t col){return my_new(row*col, sizeof(float));}

inline void set_f_matrix2(float *matrix, int64_t row, int64_t col, int64_t i, int64_t j, float value){matrix[i*col+j]=value;}

inline float get_f_matrix2(float *matrix, int64_t row, int64_t col, int64_t i, int64_t j){return matrix[i*col+j];}

inline double *new_d_matrix2(int64_t row, int64_t col){return my_new(row*col, sizeof(double));}

inline void set_d_matrix2(double *matrix, int64_t row, int64_t col, int64_t i, int64_t j, double value){matrix[i*col+j]=value;}

inline double get_d_matrix2(double *matrix, int64_t row, int64_t col, int64_t i, int64_t j){return matrix[i*col+j];}

inline int *new_i_matrix3(int64_t dim1, int64_t dim2, int64_t dim3){return my_new(dim1*dim2*dim3, sizeof(int));}

inline void set_i_matrix3(int *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, int value){matrix[i*dim2*dim3+j*dim3+k]=value;}

inline int get_i_matrix3(int *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k){return matrix[i*dim2*dim3+j*dim3+k];}

inline int64_t *new_l_matrix3(int64_t dim1, int64_t dim2, int64_t dim3){return my_new(dim1*dim2*dim3, sizeof(int64_t));}

inline void set_l_matrix3(int64_t *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, int64_t value){matrix[i*dim2*dim3+j*dim3+k]=value;}

inline int64_t get_l_matrix3(int64_t *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k){return matrix[i*dim2*dim3+j*dim3+k];}

inline float *new_f_matrix3(int64_t dim1, int64_t dim2, int64_t dim3){return my_new(dim1*dim2*dim3, sizeof(float));}

inline void set_f_matrix3(float *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, float value){matrix[i*dim2*dim3+j*dim3+k]=value;}

inline float get_f_matrix3(float *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k){return matrix[i*dim2*dim3+j*dim3+k];}

inline double *new_d_matrix3(int64_t dim1, int64_t dim2, int64_t dim3){return my_new(dim1*dim2*dim3, sizeof(double));}

inline void set_d_matrix3(double *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k, double value){matrix[i*dim2*dim3+j*dim3+k]=value;}

inline double get_d_matrix3(double *matrix, int64_t dim1, int64_t dim2, int64_t dim3, int64_t i, int64_t j, int64_t k){return matrix[i*dim2*dim3+j*dim3+k];}

double dotProduct(double *vs1, double *vs2, int len){
    double res=0.0;
    int i, num=len/4;
#ifdef _USE_AVX
    if(num>0){
        __m256d r=_mm256_set1_pd(0.0);
        for(i=0;i<num;i++) r=_mm256_fmadd_pd(_mm256_loadu_pd(vs1+i*4), _mm256_loadu_pd(vs2+i*4), r);
        res+=r[0];
        res+=r[1];
        res+=r[2];
        res+=r[3];
    }
    for(i=num*4;i<len;i++)res+=(vs1[i]*vs2[i]);
#else
    for(i=0;i<len;i++)res+=(vs1[i]*vs2[i]);
#endif

    return res;
}

inline double norm_L2(double *vs, int len){ return sqrt(dotProduct(vs, vs, len));}

void normalizeArray(double *vs, int len){
    double total=0.0;
    int i;
    for(i=0;i<len;i++) total+=vs[i];
    for(i=0;i<len;i++) vs[i]/=total;
}

void avg_unbiased_var(int len, double *arr, double *res_avg, double *res_var){
    if(len<1){
        *(res_avg)=0.0;
        *(res_var)=1.0;
        return;
    }else if(len==1){
        *(res_avg)=arr[0];
        *(res_var)=0.0;
        return;
    }

    int i;
    double avg=0.0, var=0.0;
    for(i=0;i<len;i++) avg+=arr[i];
    avg/=(double)len;
    for(i=0;i<len;i++){
        double d=arr[i]-avg;
        var+=(d*d);
    }
    var=sqrt(var/(len-1));

    *(res_avg)=avg;
    *(res_var)=var;
}

void avg_biased_var(int len, double *arr, double *res_avg, double *res_var){
    if(len<1){
        *(res_avg)=0.0;
        *(res_var)=1.0;
        return;
    }else if(len==1){
        *(res_avg)=arr[0];
        *(res_var)=0.0;
        return;
    }

    int i;
    double avg=0.0, var=0.0;
    for(i=0;i<len;i++) avg+=arr[i];
    avg/=(double)len;
    for(i=0;i<len;i++){
        double d=arr[i]-avg;
        var+=(d*d);
    }
    var=sqrt(var/len);

    *(res_avg)=avg;
    *(res_var)=var;
}

inline double norm_pdf(double x, double mu, double sigma){return (1.0/(sqrt(2.0*M_PI)*sigma))*exp(-pow(x-mu, 2.0)/(2.0*sigma*sigma));}

inline double norm_cdf(double x, double mu, double sigma){return 0.5+0.5*erff((x-mu)/sigma/SQRT2);}

double gamm2(double a,double x,double e1,double e0){
    int n;
    double t,del,gln;
    double an,bn,c,d;

    if((x<0.0)||(a<=0)){printf("x<0.0 or a<=0.0\n");return(0);}
    if(x<e0) return(0.0);

    gln=log_gamma(a);
    if(x<(a+1.0)){
        del=1.0/a;
        t=1.0/a;
        for(n=1;n<100;n++){
            del=del*x/(a+n);
            t=t+del;
            if(fabs(del)<fabs(t)*e1){
                t=t*exp(-x+a*log(x)-gln);
                return(t);
            }
        }
        printf("iteration too many times\n");
        return(0);
    }else{
        bn=x+1.0-a;
        c=1.0/e0;
        d=1.0/bn;
        t=d;
        for(n=1;n<100;n++){
            an=n*(a-n);
            bn=bn+2.0;
            d=an*d+bn;
            c=bn+an/c;
            if(fabs(d)<e0)
                d=e0;
            if(fabs(c)<e0)
                c=e0;
            d=1.0/d;
            del=d*c;
            t=t*del;
            if(fabs(del-1.0)<e1){
                t=exp(-x+a*log(x)-gln)*t;
                t=1.0-t;
                return(t);
            }

        }
        printf("iteration too many times\n");
        return(0);
    }
}

inline double chi2_pdf(double x, int k){return (1.0/(pow(2.0, k/2.0)*exp(log_gamma(k/2.0))))*pow(x, k/2.0-1.0)*exp(-x/2.0);}

inline double chi2_cdf(double x, int k){return x<0.0 ? 0.0:gamm2(k/2.0,x/2.0,1.0e-6,1.0e-30);}

//equals: gsl_cdf_chisq_Q in(#include <gsl/gsl_cdf.h>) (gcc lib: -lgsl -lgslcblas)
inline double chi2_cdf_upper(double x, int k){return x<0.0 ? 1.0 : (exp(-x/2)*pow(x, k/2.0-1)*2.0/(pow(2, k/2.0)*tgamma(k/2.0)));}

int matrix_multiply_row1;
int matrix_multiply_col1;
int matrix_multiply_row2;
int matrix_multiply_col2;
double *matrix_multiply_arr1;
double *matrix_multiply_arr2;
double *matrix_multiply_res1;

static void matrix_multi_one(int index){
    int row1=matrix_multiply_row1;
    int col1=matrix_multiply_col1;
    int row2=matrix_multiply_row2;
    int col2=matrix_multiply_col2;
    double *arr1=matrix_multiply_arr1;
    double *arr2=matrix_multiply_arr2;
    double *res1=matrix_multiply_res1;

    int i, j, k, num;

    for(i=index;i<row1;i+=thread_num){
        double *tmp1=arr1+i*col1;
        for(j=0;j<col2;j++){
            double *tmp2=arr2+j*row2;
            double value=0.0;
            //--
            num=col1/4;
#ifdef _USE_AVX
            if(num>0){
                __m256d r=_mm256_set1_pd(0.0);
                for(k=0;k<num;k++){
                    r=_mm256_fmadd_pd(_mm256_loadu_pd(tmp1+k*4), _mm256_loadu_pd(tmp2+k*4), r);
                }
                value+=r[0];
                value+=r[1];
                value+=r[2];
                value+=r[3];
            }
            for(k=num*4;k<col1;k++) value+=tmp1[k]*tmp2[k];
#else
            for(k=0;k<col1;k++) value+=tmp1[k]*tmp2[k];
#endif
            //--
            res1[i*col2+j]=value;
        }
    }
}

void matrix_multi(double *arr1, double *arr2, double *res1, int row1, int col1, int row2, int col2){
    if(col1!=row2){
        fprintf(stderr, "[error]:col1!=row2, please check!\n");
        exit(0);
    }

    int i, j;

    double *arr3=calloc((int64_t)row2*(int64_t)col2, sizeof(double));
    for(i=0;i<row2;i++){
        for(j=0;j<col2;j++) arr3[j*row2+i]=arr2[i*col2+j];
    }

    memset(res1, 0, row1*col2*sizeof(double));

    matrix_multiply_row1=row1;
    matrix_multiply_col1=col1;
    matrix_multiply_row2=row2;
    matrix_multiply_col2=col2;
    matrix_multiply_arr1=arr1;
    matrix_multiply_arr2=arr3;
    matrix_multiply_res1=res1;

    if(thread_num<2){
        thread_num=1;
        matrix_multi_one(0);
    }else{
        ThreadPool *pool=new_thread_pool(thread_num);
        for(i=0;i<thread_num;i++) thread_pool_add_worker(pool, (void *(*)(void *))matrix_multi_one, (void *)i);
        thread_pool_invoke_all(pool);
        free_thread_pool(pool);
    }

    free(arr3);
}

//=========================================================== optimize mcmc

int my_mcmc_min_max_reject_num;
int my_mcmc_min_sample_type;
int my_mcmc_min_l_vec;
double *my_mcmc_min_lowbound;
double *my_mcmc_min_upbound;
double (*my_mcmc_min_func)(double *, void *);
void *my_mcmc_min_data;
double **my_mcmc_min_vecs;
double *my_mcmc_min_likes;

/*
 * max_reject_num:   if reject number is large than this, it should be convergence, break and return
 * sample_type:      0: sample_gaussian 1: sample_uniform
 */
double my_mcmc_min(int max_reject_num, int sample_type, int l_vec, double *vec, void *data, double *lowbound, double *upbound, double (*func)(double *, void *)){
    my_mcmc_min_max_reject_num=max_reject_num;
    my_mcmc_min_sample_type=sample_type;
    my_mcmc_min_l_vec=l_vec;
    my_mcmc_min_lowbound=lowbound;
    my_mcmc_min_upbound=upbound;
    my_mcmc_min_func=func;
    my_mcmc_min_data=data;

    my_mcmc_min_vecs= my_new(thread_num, sizeof(double *));
    my_mcmc_min_likes= my_new(thread_num, sizeof(double));

    int i;

    double like=func(vec, data);
    for(i=0;i<thread_num;i++){
        my_mcmc_min_vecs[i]= my_new(l_vec, sizeof(double));
        memcpy(my_mcmc_min_vecs[i], vec, l_vec*sizeof(double));
        my_mcmc_min_likes[i]=like;
        thread_pool_add_worker(thread_pool, (void *(*)(void *))my_mcmc_min_one, (void *)i);
    }
    thread_pool_invoke_all(thread_pool);

    for(i=0;i<thread_num;i++){
        if(like>my_mcmc_min_likes[i]){
            like=my_mcmc_min_likes[i];
            memcpy(vec, my_mcmc_min_vecs[i], l_vec*sizeof(double));
        }
        free(my_mcmc_min_vecs[i]);
    }
    free(my_mcmc_min_vecs);
    free(my_mcmc_min_likes);

    return like;
}

void my_mcmc_min_one(int index){
    MyRand *rand= new_my_rand2(index);

    double *vec=my_new(my_mcmc_min_l_vec, sizeof(double));

    int num=0;
    while(num<my_mcmc_min_max_reject_num){
        if(my_mcmc_min_sample_type==0) my_mcmc_min_sample_gaussian(rand, my_mcmc_min_l_vec, my_mcmc_min_vecs[index], my_mcmc_min_lowbound, my_mcmc_min_upbound, vec);
        else if(my_mcmc_min_sample_type==1) my_mcmc_min_sample_uniform(rand, my_mcmc_min_l_vec, my_mcmc_min_vecs[index], my_mcmc_min_lowbound, my_mcmc_min_upbound, vec);
        double tmp_like=my_mcmc_min_func(vec, my_mcmc_min_data);
        if(tmp_like<my_mcmc_min_likes[index]){
            if(index==0) fprintf(stdout, "like=%f\n", tmp_like);
            my_mcmc_min_likes[index]=tmp_like;
            memcpy(my_mcmc_min_vecs[index], vec, my_mcmc_min_l_vec*sizeof(double));
            num=0;
        }else{
            num++;
        }
    }

    free(vec);
    free(rand);
}

void my_mcmc_min_sample_gaussian(MyRand  *rand, int l_vec, double *vec, double *lowbound, double *upbound, double *res){
    int i;
    for(i=0;i<l_vec;i++){
        double x=vec[i], y=0.0;
        if(lowbound!=NULL && upbound!=NULL){
            double low=lowbound[i];
            double up=upbound[i];
            double var=(up-low+1.0)/100;
            y=x+my_rand_next_gaussian(rand)*var;
            if(y<low) y=low;
            if(y>up) y=up;
        }else if(lowbound==NULL && upbound!=NULL){
            double up=upbound[i];
            double var=(up-x+1.0)/100;
            y=x+my_rand_next_gaussian(rand)*var;
            if(y>up) y=up;
        }else if(lowbound!=NULL && upbound==NULL){
            double low=lowbound[i];
            double var=(x-low+1.0)/100;
            y=x+my_rand_next_gaussian(rand)*var;
            if(y<low) y=low;
        }else{
            double var=1.0;
            y=x+my_rand_next_gaussian(rand)*var;
        }
        res[i]=y;
    }
}

void my_mcmc_min_sample_uniform(MyRand  *rand, int l_vec, double *vec, double *lowbound, double *upbound, double *res){
    if(lowbound==NULL || upbound==NULL){
        my_mcmc_min_sample_gaussian(rand, l_vec, vec, lowbound, upbound, res);
        return;
    }

    int i;
    for(i=0;i<l_vec;i++){
        double low=lowbound[i];
        double up=upbound[i];
        res[i]=low+my_rand_next_double(rand)*(up-low);
    }
}

//=========================================================== optimize powell (not support multi-thread)

#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SQR(a) (a)*(a)
#define FMAX(a,b) (a)>(b) ? (a):(b)
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

int ncom;
double *pcom,*xicom,(*nrfunc)(double []);

/*
 * p : 向量初始值。退出时，为最优化时的向量
 * xi ：搜索方向矩阵，初始化：n个单位向量
 * n ：向量的个数
 * ftol ：如果在一次迭代中，(func_pre-func_cur)/((fabg(func_pre)+fabs(func_cur))/2.0) < ftol，则跳出循环
 * iter ：返回实际迭代次数
 * fret ：返回时，最优化的向量对应的函数值
 * func ：似然函数
 */
void powell(double p[], int n, double ftol, int *iter, double *fret, double (*func)(double [])){
    int i,ibig,j;
    double del,fp,fptt,t,*pt,*ptt,*xit;

    pt= my_new(n, sizeof(double));
    ptt= my_new(n, sizeof(double));
    xit= my_new(n, sizeof(double));
    *fret=(*func)(p);

    double unit=1.0/ sqrt((double)n);
    double **xi= my_new(n, sizeof(double *));
    for(i=0;i<n;i++){
        xi[i]= my_new(n, sizeof(double));
        for(j=0;j<n;j++) xi[i][j]=unit;
    }

    for (j=0;j<n;j++) pt[j]=p[j];

    for (*iter=1;;++(*iter)) {
        fp=(*fret);
        ibig=0;
        del=0.0;
        for (i=0;i<n;i++) {
            for (j=0;j<n;j++) xit[j]=xi[j][i];
            fptt=(*fret);
            linmin(p,xit,n,fret,func);
            if (fptt-(*fret) > del) {
                del=fptt-(*fret);
                ibig=i;
            }
        }
        if (2.0*(fp-(*fret)) <= ftol*(fabs(fp)+fabs(*fret))+1.0e-25) {
            free(xit);
            free(ptt);
            free(pt);
            freeArray(n, xi);
            return;
        }
        if (*iter == 200){
            fprintf(stderr, "powell exceeding maximum iterations.");
            exit(1);
        }
        for (j=0;j<n;j++) {
            ptt[j]=2.0*p[j]-pt[j];
            xit[j]=p[j]-pt[j];
            pt[j]=p[j];
        }
        fptt=(*func)(ptt);
        if (fptt < fp) {
            t=2.0*(fp-2.0*(*fret)+fptt)*SQR(fp-(*fret)-del)-del*SQR(fp-fptt);
            if (t < 0.0) {
                linmin(p,xit,n,fret,func);
                for (j=0;j<n;j++) {
                    xi[j][ibig]=xi[j][n-1];
                    xi[j][n-1]=xit[j];
                }
            }
        }
    }
}

void linmin(double p[], double xi[], int n, double *fret, double (*func)(double [])){
    int j;
    double xx,xmin,fx,fb,fa,bx,ax;

    ncom=n;
    pcom= my_new(n, sizeof(double));
    xicom= my_new(n, sizeof(double));
    nrfunc=func;
    for (j=0;j<n;j++) {
        pcom[j]=p[j];
        xicom[j]=xi[j];
    }
    ax=0.0;
    xx=1.0;
    mnbrak(&ax,&xx,&bx,&fa,&fx,&fb,f1dim);
    *fret=brent(ax,xx,bx,f1dim,2.0e-4,&xmin);
    for (j=0;j<n;j++) {
        xi[j] *= xmin;
        p[j] += xi[j];
    }
    free(xicom);
    free(pcom);
}

double f1dim(double x){
    int j;
    double f,*xt;

    xt= my_new(ncom, sizeof(double));
    for (j=0;j<ncom;j++) xt[j]=pcom[j]+x*xicom[j];
    f=(*nrfunc)(xt);
    free(xt);
    return f;
}

void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, double (*func)(double)){
    double ulim,u,r,q,fu,dum;

    *fa=(*func)(*ax);
    *fb=(*func)(*bx);
    if (*fb > *fa) {
        SHFT(dum,*ax,*bx,dum)
        SHFT(dum,*fb,*fa,dum)
    }
    *cx=(*bx)+1.618034*(*bx-*ax);
    *fc=(*func)(*cx);
    while (*fb > *fc) {
        r=(*bx-*ax)*(*fb-*fc);
        q=(*bx-*cx)*(*fb-*fa);
        u=(*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
                (2.0*SIGN(FMAX(fabs(q-r),1.0e-20),q-r));
        ulim=(*bx)+100.0*(*cx-*bx);
        if ((*bx-u)*(u-*cx) > 0.0) {
            fu=(*func)(u);
            if (fu < *fc) {
                *ax=(*bx);
                *bx=u;
                *fa=(*fb);
                *fb=fu;
                return;
            } else if (fu > *fb) {
                *cx=u;
                *fc=fu;
                return;
            }
            u=(*cx)+1.618034*(*cx-*bx);
            fu=(*func)(u);
        } else if ((*cx-u)*(u-ulim) > 0.0) {
            fu=(*func)(u);
            if (fu < *fc) {
                SHFT(*bx,*cx,u,*cx+1.618034*(*cx-*bx))
                SHFT(*fb,*fc,fu,(*func)(u))
            }
        } else if ((u-ulim)*(ulim-*cx) >= 0.0) {
            u=ulim;
            fu=(*func)(u);
        } else {
            u=(*cx)+1.618034*(*cx-*bx);
            fu=(*func)(u);
        }
        SHFT(*ax,*bx,*cx,u)
        SHFT(*fa,*fb,*fc,fu)
    }
}

double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin){
    int iter;
    double a,b,d,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm;
    double e=0.0;

    a=(ax < cx ? ax : cx);
    b=(ax > cx ? ax : cx);
    x=w=v=bx;
    fw=fv=fx=(*f)(x);
    for (iter=1;iter<=100;iter++) {
        xm=0.5*(a+b);
        tol2=2.0*(tol1=tol*fabs(x)+1.0e-10);
        if (fabs(x-xm) <= (tol2-0.5*(b-a))) {
            *xmin=x;
            return fx;
        }
        if (fabs(e) > tol1) {
            r=(x-w)*(fx-fv);
            q=(x-v)*(fx-fw);
            p=(x-v)*q-(x-w)*r;
            q=2.0*(q-r);
            if (q > 0.0) p = -p;
            q=fabs(q);
            etemp=e;
            e=d;
            if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
                d=0.3819660*(e=(x >= xm ? a-x : b-x));
            else {
                d=p/q;
                u=x+d;
                if (u-a < tol2 || b-u < tol2)
                    d=SIGN(tol1,xm-x);
            }
        } else {
            d=0.3819660*(e=(x >= xm ? a-x : b-x));
        }
        u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
        fu=(*f)(u);
        if (fu <= fx) {
            if (u >= x) a=x; else b=x;
            SHFT(v,w,x,u)
            SHFT(fv,fw,fx,fu)
        } else {
            if (u < x) a=u; else b=u;
            if (fu <= fw || w == x) {
                v=w;
                w=u;
                fv=fw;
                fw=fu;
            } else if (fu <= fv || v == x || v == w) {
                v=u;
                fv=fu;
            }
        }
    }

    fprintf(stderr, "Too many iterations in brent!\n");
    exit(1);

    *xmin=x;
    return fx;
}

//=========================================================== optimize least squares

//------------------------------ fit gaussian

LeastSquaresEvaluation *gaussian_fit(int size, double *xs, double *ys){
    int n_par=3;
    double *start_points= gaussian_guess_start_points(size, xs, ys);
    LeastSquaresEvaluation *evaluation= leastSquares_optimize(size, xs, ys, n_par, start_points, gaussian_func, gaussian_gradient);
    free(start_points);
    return evaluation;
}

//-- n_par=3 param[0]=scale param[1]=mu param[2]=sigma
inline double gaussian_func(double x, int n_par, double *param){
    if(n_par!=3){
        char buff[100];
        fprintf(stderr, "[error]%s: gaussian function para's num must be 3!\n", debug_info(buff));
        exit(0);
    }
    double diff = x - param[1];
    double i2s2 = 1.0 / (2.0 * param[2] * param[2]);
    return param[0]* exp(-diff*diff*i2s2);  //Gaussian.value(diff, param[0], i2s2);
}

//-- n_par=3 param[0]=scale param[1]=mu param[2]=sigma
double *gaussian_gradient(double x, int n_par, double *param, double(*func)(double,int,double*)){
    if(n_par!=3){
        char buff[100];
        fprintf(stderr, "[error]%s: gaussian function para's num must be 3!\n", debug_info(buff));
        exit(0);
    }
    double norm = param[0];
    double diff = x - param[1];
    double sigma = param[2];
    double i2s2 = 1 / (2 * sigma * sigma);

    double n = 1.0*exp(-diff*diff*i2s2);    //Gaussian.value(diff, 1, i2s2);
    double m = norm * n * 2 * i2s2 * diff;
    double s = m * diff / sigma;

    double *res= my_new(n_par, sizeof(double));
    res[0]=n;
    res[1]=m;
    res[2]=s;
    return res;
}

double *gaussian_guess_start_points(int size, double *xs, double *ys){
    int i, j, k;

    int maxYIdx, max_tmp;
    for(i=0;i<size;i++){
        if(i==0 || max_tmp<ys[i]){
            maxYIdx=i;
            max_tmp=ys[i];
        }
    }

    double m=xs[maxYIdx];
    double n=ys[maxYIdx];

    double fwhmApprox;

    double halfY = n + ((m - n) / 2);
    int is_out_of_range;
    double fwhmX1 = gaussian_interpolateXAtY(size, xs, ys, maxYIdx, -1, halfY, &is_out_of_range);   //interpolateXAtY(points, maxYIdx, -1, halfY);
    if(is_out_of_range){
        fwhmApprox=xs[size-1]-xs[0];
    }else{
        double fwhmX2 = gaussian_interpolateXAtY(size, xs, ys, maxYIdx, 1, halfY, &is_out_of_range);    //interpolateXAtY(points, maxYIdx, 1, halfY);
        if(is_out_of_range){
            fwhmApprox=xs[size-1]-xs[0];
        }else{
            fwhmApprox = fwhmX2 - fwhmX1;
        }
    }

    double s = fwhmApprox / (2.0 * sqrt(2.0 * log(2.0)));

    double *res= my_new(3, sizeof(double));
    res[0]=n;
    res[1]=m;
    res[2]=s;
    return res;
}

double gaussian_interpolateXAtY(int size, double *xs, double *ys, int startIdx, int idxStep, double y, int *is_out_of_range){
    int i, j;

    *(is_out_of_range) = 0;

    for (i = startIdx; idxStep < 0 ? i + idxStep >= 0 : i + idxStep < size; i += idxStep) {
        double x1 = xs[i];
        double x2 = xs[i + idxStep];
        double y1 = ys[i];
        double y2 = ys[i + idxStep];
        if ((y >= y1 && y <= y2) || (y >= y2 && y <= y1)) {
            if (y == y1) return x1;
            if (y == y2) return x2;
            if (idxStep < 0) {
                double tmp = x1;
                x1 = x2;
                x2 = tmp;

                tmp = y1;
                y1 = y2;
                y2 = tmp;
            }
            //return p1.getX() + (((y - p1.getY()) * (p2.getX() - p1.getX())) / (p2.getY() - p1.getY()));
            return x1 + (((y - y1) * (x2 - x1)) / (y2 - y1));
        }
    }

    *(is_out_of_range) = 1;
}

//------------------------------ least squares

LeastSquaresEvaluation *leastSquares_optimize(int size, double *xs, double *ys, int n_par, double *start_points, double(*func)(double,int,double*), double*(*gradient)(double,int,double*,double(*)(double,int,double*))){
    //-- check xs is sorted
    int is_new_data=0;
    {
        int i;
        for(i=0;i<(size-1);i++){
            double a=xs[i];
            double b=xs[i+1];
            if(a>=b){
                is_new_data=1;
                break;
            }
        }
        if(is_new_data){
            char *buff= my_new(1000, sizeof(char));
            char **tmps= my_new(size, sizeof(char *));
            for(i=0;i<size;i++) tmps[i]= str_copy_with_len(buff, sprintf(buff, "%f\t%d", xs[i], i));
            java_sort_void(tmps, 0, size, (int (*)(void *, void *))leastSquares_compare_xs);
            free(buff);

            double *tmp_xs= my_new(size, sizeof(double));
            double *tmp_ys= my_new(size, sizeof(double));
            for(i=0;i<size;i++){
                char *str=tmps[i];
                int j=0;
                while(str[j++]!='\t');
                int index= atoi(str+j);
                tmp_xs[i]=xs[index];
                tmp_ys[i]=ys[index];
                free(str);
            }
            free(tmps);

            xs=tmp_xs;
            ys=tmp_ys;
        }
    }

    if(gradient==NULL) gradient=leastSquares_default_gradient;

    int iterationCounter=0;
    int evaluationCounter=0;

    /* configuration parameters */
    /** Positive input variable used in determining the initial step bound. */
    double initialStepBoundFactor=100.0;
    /** Desired relative error in the sum of squares. */
    double costRelativeTolerance=1e-10;
    /**  Desired relative error in the approximate solution parameters. */
    double parRelativeTolerance=1e-10;
    /** Desired max cosine on the orthogonality between the function vector
     * and the columns of the jacobian. */
    double orthoTolerance=1e-10;
    /** Threshold for QR ranking. */
    double qrRankingThreshold=2.2250738585072014E-308;

    double EPSILON=1.1102230246251565E-16;
    double TWO_EPS = 2 * EPSILON;

    int i, j, k;

    int nR=size;
    int nC=n_par;
    int solvedCols=nR<nC ? nR:nC;

    //Levenberg-Marquardt parameter.
    double lmPar = 0;

    //Parameters evolution direction associated with lmPar.
    double *lmDir = my_new(nC, sizeof(double)); //new double[nC];

    // local point
    double   delta   = 0;
    double   xNorm   = 0;
    double *diag    = my_new(nC, sizeof(double));   //new double[nC];
    double *oldX    = my_new(nC, sizeof(double));   //new double[nC];
    double *oldRes  = my_new(nR, sizeof(double));   //new double[nR];
    double *qtf     = my_new(nR, sizeof(double));   //new double[nR];
    double *work1   = my_new(nC, sizeof(double));   //new double[nC];
    double *work2   = my_new(nC, sizeof(double));   //new double[nC];
    double *work3   = my_new(nC, sizeof(double));   //new double[nC];

    evaluationCounter++;
    LeastSquaresEvaluation *current= leastSquares_evaluate(size, xs, ys, n_par, start_points, func, gradient);
    double *currentResiduals=current->residuals;
    double currentCost=current->cost;

    //double *currentPoint=current->points;
    double *currentPoint= my_new(nC, sizeof(double));
    memcpy(currentPoint, current->points, nC*sizeof(double));

    double *weightedResidual= my_new(nR, sizeof(double));
    double *tmpVec= my_new(nR, sizeof(double));

    int firstIteration = true;
    while (true) {
        iterationCounter++; //iterationCounter.incrementCount();

        LeastSquaresEvaluation *previous=current;   //Evaluation previous = current;

        // QR decomposition of the jacobian matrix
        LeastSquaresInternalData *internalData= leastSquares_qrDecomposition(current->n_data, current->n_par, current->jacobian, solvedCols, qrRankingThreshold);   //InternalData internalData = qrDecomposition(current.getJacobian(), solvedCols);
        double **weightedJacobian=internalData->weightedJacobian;   //double[][] weightedJacobian = internalData.weightedJacobian;
        int *permutation=internalData->permutation;  //int[] permutation = internalData.permutation;
        double *diagR=internalData->diagR;  //double[] diagR = internalData.diagR;
        double *jacNorm=internalData->jacNorm;  //double[] jacNorm = internalData.jacNorm;

        //residuals already have weights applied
        memcpy(weightedResidual, currentResiduals, nR*sizeof(double));  //double* weightedResidual = currentResiduals;
        for (i = 0; i < nR; i++) {
            qtf[i] = weightedResidual[i];
        }

        // compute Qt.res
        leastSquares_qTy(qtf, internalData);

        // now we don't need Q anymore,
        // so let jacobian contain the R matrix with its diagonal elements
        for (k = 0; k < solvedCols; ++k) {
            int pk = permutation[k];
            weightedJacobian[k][pk] = diagR[pk];
        }

        if (firstIteration) {
            // scale the point according to the norms of the columns
            // of the initial jacobian
            xNorm = 0;
            for (k = 0; k < nC; ++k) {
                double dk = jacNorm[k];
                if (dk == 0) {
                    dk = 1.0;
                }
                double xk = dk * currentPoint[k];
                xNorm  += xk * xk;
                diag[k] = dk;
            }
            xNorm = sqrt(xNorm);

            // initialize the step bound delta
            delta = (xNorm == 0) ? initialStepBoundFactor : (initialStepBoundFactor * xNorm);
        }

        // check orthogonality between function vector and jacobian columns
        double maxCosine = 0;
        if (currentCost != 0) {
            for (j = 0; j < solvedCols; ++j) {
                int    pj = permutation[j];
                double s  = jacNorm[pj];
                if (s != 0) {
                    double sum = 0;
                    int i;
                    for (i = 0; i <= j; ++i) {
                        sum += weightedJacobian[i][pj] * qtf[i];
                    }
                    maxCosine = max(maxCosine, fabs(sum) / (s * currentCost));
                }
            }
        }
        if (maxCosine <= orthoTolerance) {
            // Convergence has been reached.

            free(lmDir);
            free(diag);
            free(oldX);
            free(oldRes);
            free(qtf);
            free(work1);
            free(work2);
            free(work3);
            free(currentPoint);
            free(weightedResidual);
            free(tmpVec);
            free_LeastSquaresInternalData(internalData);
            if(is_new_data){
                free(xs);
                free(ys);
            }

            current->evaluations=evaluationCounter;
            current->iterations=iterationCounter;
            return current; //return new OptimumImpl(current,evaluationCounter.getCount(), iterationCounter.getCount());
        }

        // rescale if necessary
        for (j = 0; j < nC; ++j) {
            diag[j] = max(diag[j], jacNorm[j]);
        }

        // Inner loop.
        double ratio;
        LeastSquaresEvaluation *new_evaluation=NULL;
        for (ratio = 0; ratio < 1.0e-4;) {
            // save the state
            for (j = 0; j < solvedCols; ++j) {
                int pj = permutation[j];
                oldX[pj] = currentPoint[pj];
            }
            double previousCost = currentCost;
            //-- sweep weightedResidual and oldRes
            if(1){
                memcpy(tmpVec, weightedResidual, nR*sizeof(double));
                memcpy(weightedResidual, oldRes, nR*sizeof(double));
                memcpy(oldRes, tmpVec, nR*sizeof(double));
            }else{
                double* tmpVec = weightedResidual;
                weightedResidual = oldRes;
                oldRes    = tmpVec;
            }

            // determine the Levenberg-Marquardt parameter
            lmPar = leastSquares_determineLMParameter(qtf, delta, diag, internalData, solvedCols, work1, work2, work3, lmDir, lmPar);

            // compute the new point and the norm of the evolution direction
            double lmNorm = 0;
            for (j = 0; j < solvedCols; ++j) {
                int pj = permutation[j];
                lmDir[pj] = -lmDir[pj];
                currentPoint[pj] = oldX[pj] + lmDir[pj];
                double s = diag[pj] * lmDir[pj];
                lmNorm  += s * s;
            }
            lmNorm = sqrt(lmNorm);
            // on the first iteration, adjust the initial step bound.
            if (firstIteration) {
                delta = min(delta, lmNorm);
            }

            // Evaluate the function at x + p and calculate its norm.
            evaluationCounter++;    //evaluationCounter.incrementCount();
            if(new_evaluation) free_LeastSquaresEvaluation(new_evaluation);
            current = leastSquares_evaluate(size, xs, ys, n_par, currentPoint, func, gradient); //problem.evaluate(new ArrayRealVector(currentPoint));
            new_evaluation=current;
            currentResiduals = current->residuals;  //.getResiduals().toArray();
            currentCost = current->cost;    //.getCost();
            //currentPoint = current->points; //.getPoint().toArray();
            //memcpy(currentPoint, current->points, n_par*sizeof(double));

            // compute the scaled actual reduction
            double actRed = -1.0;
            if (0.1 * currentCost < previousCost) {
                double r = currentCost / previousCost;
                actRed = 1.0 - r * r;
            }

            // compute the scaled predicted reduction
            // and the scaled directional derivative
            for (j = 0; j < solvedCols; ++j) {
                int pj = permutation[j];
                double dirJ = lmDir[pj];
                work1[j] = 0;
                for (i = 0; i <= j; ++i) {
                    work1[i] += weightedJacobian[i][pj] * dirJ;
                }
            }
            double coeff1 = 0;
            for (j = 0; j < solvedCols; ++j) {
                coeff1 += work1[j] * work1[j];
            }
            double pc2 = previousCost * previousCost;
            coeff1 /= pc2;
            double coeff2 = lmPar * lmNorm * lmNorm / pc2;
            double preRed = coeff1 + 2 * coeff2;
            double dirDer = -(coeff1 + coeff2);

            // ratio of the actual to the predicted reduction
            ratio = (preRed == 0) ? 0 : (actRed / preRed);

            // update the step bound
            if (ratio <= 0.25) {
                double tmp =
                        (actRed < 0) ? (0.5 * dirDer / (dirDer + 0.5 * actRed)) : 0.5;
                if ((0.1 * currentCost >= previousCost) || (tmp < 0.1)) {
                    tmp = 0.1;
                }
                delta = tmp * min(delta, 10.0 * lmNorm);
                lmPar /= tmp;
            } else if ((lmPar == 0) || (ratio >= 0.75)) {
                delta = 2 * lmNorm;
                lmPar *= 0.5;
            }

            // test for successful iteration.
            if (ratio >= 1.0e-4) {
                // successful iteration, update the norm
                firstIteration = false;
                xNorm = 0;
                for (k = 0; k < nC; ++k) {
                    double xK = diag[k] * currentPoint[k];
                    xNorm += xK * xK;
                }
                xNorm = sqrt(xNorm);

//                // tests for convergence.
//                if (checker != null && checker.converged(iterationCounter.getCount(), previous, current)) {
//                    return new OptimumImpl(current, evaluationCounter.getCount(), iterationCounter.getCount());
//                }
            } else {
                // failed iteration, reset the previous values
                currentCost = previousCost;
                for (j = 0; j < solvedCols; ++j) {
                    int pj = permutation[j];
                    currentPoint[pj] = oldX[pj];
                }
                //-- sweep weightedResidual and oldRes
                if(1){
                    memcpy(tmpVec, weightedResidual, nR*sizeof(double));
                    memcpy(weightedResidual, oldRes, nR*sizeof(double));
                    memcpy(oldRes, tmpVec, nR*sizeof(double));
                }else{
                    double *tmpVec    = weightedResidual;
                    weightedResidual = oldRes;
                    oldRes    = tmpVec;
                }
                // Reset "current" to previous values.
                free_LeastSquaresEvaluation(current);
                new_evaluation=NULL;
                current = previous;
            }

            // Default convergence criteria.
            if ((fabs(actRed) <= costRelativeTolerance && preRed <= costRelativeTolerance && ratio <= 2.0) || delta <= parRelativeTolerance * xNorm) {
                free(lmDir);
                free(diag);
                free(oldX);
                free(oldRes);
                free(qtf);
                free(work1);
                free(work2);
                free(work3);
                free(currentPoint);
                free(weightedResidual);
                free(tmpVec);
                free_LeastSquaresInternalData(internalData);
                if(is_new_data){
                    free(xs);
                    free(ys);
                }

                if(previous!=current) free_LeastSquaresEvaluation(previous);

                current->evaluations=evaluationCounter;
                current->iterations=iterationCounter;
                return current; //new OptimumImpl(current, evaluationCounter.getCount(), iterationCounter.getCount());
            }

            // tests for termination and stringent tolerances
            if (fabs(actRed) <= TWO_EPS && preRed <= TWO_EPS && ratio <= 2.0) {
                //throw new ConvergenceException(LocalizedFormats.TOO_SMALL_COST_RELATIVE_TOLERANCE, costRelativeTolerance);
                fprintf(stderr, "least_squares_exception:TOO_SMALL_COST_RELATIVE_TOLERANCE, %f\n", costRelativeTolerance);
                exit(0);
            } else if (delta <= TWO_EPS * xNorm) {
                //throw new ConvergenceException(LocalizedFormats.TOO_SMALL_PARAMETERS_RELATIVE_TOLERANCE, parRelativeTolerance);
                fprintf(stderr, "least_squares_exception:TOO_SMALL_PARAMETERS_RELATIVE_TOLERANCE, %f\n", parRelativeTolerance);
                exit(0);
            } else if (maxCosine <= TWO_EPS) {
                //throw new ConvergenceException(LocalizedFormats.TOO_SMALL_ORTHOGONALITY_TOLERANCE, orthoTolerance);
                fprintf(stderr, "least_squares_exception:TOO_SMALL_ORTHOGONALITY_TOLERANCE, %f\n", orthoTolerance);
                exit(0);
            }
        }

        free_LeastSquaresInternalData(internalData);
        if(previous!=current) free_LeastSquaresEvaluation(previous);
    }

    return NULL;
}

LeastSquaresEvaluation *leastSquares_evaluate(int size, double *xs, double *ys, int n_par, double *points, double(*func)(double,int,double*), double*(*gradient)(double,int,double*,double(*)(double,int,double*))){
    LeastSquaresEvaluation *et= my_new(1, sizeof(LeastSquaresEvaluation));

    int i;

    et->n_data=size;
    et->n_par=n_par;

    et->value= my_new(size, sizeof(double));
    et->points= my_new(n_par, sizeof(double));
    memcpy(et->points, points, n_par*sizeof(double));

    et->jacobian= my_new(size, sizeof(double *));
    et->residuals= my_new(size, sizeof(double));

    et->cost=0.0;
    for(i=0;i<size;i++){
        et->value[i]=func(xs[i], n_par, points);
        et->jacobian[i]= gradient(xs[i], n_par, points, func);
        et->residuals[i]=ys[i]-et->value[i];
        et->cost+=(et->residuals[i])*(et->residuals[i]);
    }
    et->cost= sqrt(et->cost);

    return et;
}

void free_LeastSquaresEvaluation(LeastSquaresEvaluation *et){
    int i;
    free(et->value);
    free(et->points);
    free(et->residuals);
    for(i=0;i<et->n_data;i++) free(et->jacobian[i]);
    free(et->jacobian);
    free(et);
}

double *leastSquares_default_gradient(double x, int n_par, double *param, double(*func)(double,int,double*)){
    double *res= my_new(n_par, sizeof(double));

    int i, j, k;

    double eh, eh0=1e-8;

    double *param1= my_new(n_par, sizeof(double));
    double *param2= my_new(n_par, sizeof(double));

    for(i=0;i<n_par;i++){
        memcpy(param1, param, n_par*sizeof(double));
        memcpy(param2, param, n_par*sizeof(double));
        eh=pow(eh0*(fabs(param[i])+1), 0.67);
        param1[i]-=eh;
        param2[i]+=eh;
        res[i]=(func(x, n_par, param2)-func(x, n_par, param1))/(2.0*eh);
    }

    free(param1);
    free(param2);

    return res;
}

LeastSquaresInternalData *leastSquares_qrDecomposition(int n_data, int n_par, double **jacobian, int solvedCols, double qrRankingThreshold){
    LeastSquaresInternalData *ld= my_new(1, sizeof(LeastSquaresInternalData));
    ld->n_data=n_data;
    ld->n_par=n_par;

    int i, j, k;

    double **weightedJacobian= my_new(n_data, sizeof(double *));
    for(i=0;i<n_data;i++){
        weightedJacobian[i]= my_new(n_par, sizeof(double));
        for(j=0;j<n_par;j++){
            weightedJacobian[i][j]=-jacobian[i][j];
        }
    }

    int nR=n_data;
    int nC=n_par;

    int* permutation = my_new(nC, sizeof(int)); //new int[nC];
    double* diagR = my_new(nC, sizeof(double)); //new double[nC];
    double* jacNorm = my_new(nC, sizeof(double));   //new double[nC];
    double* beta = my_new(nC, sizeof(double));  //new double[nC];

    // initializations
    for (k = 0; k < nC; ++k) {
        permutation[k] = k;
        double norm2 = 0;
        for (i = 0; i < nR; ++i) {
            double akk = weightedJacobian[i][k];
            norm2 += akk * akk;
        }
        jacNorm[k] = sqrt(norm2);
    }

    // transform the matrix column after column
    for (k = 0; k < nC; ++k) {

        // select the column with the greatest norm on active components
        int nextColumn = -1;
        double ak2 = NEGATIVE_INFINITY; //Double.NEGATIVE_INFINITY;
        for (i = k; i < nC; ++i) {
            double norm2 = 0;
            for (j = k; j < nR; ++j) {
                double aki = weightedJacobian[j][permutation[i]];
                norm2 += aki * aki;
            }
            if (isinf(norm2) || isnan(norm2)) {
                //throw new ConvergenceException(LocalizedFormats.UNABLE_TO_PERFORM_QR_DECOMPOSITION_ON_JACOBIAN, nR, nC);
                fprintf(stderr, "[least_squares_exception]:UNABLE_TO_PERFORM_QR_DECOMPOSITION_ON_JACOBIAN %d %d", nR, nC);
                exit(0);
            }
            if (norm2 > ak2) {
                nextColumn = i;
                ak2        = norm2;
            }
        }
        if (ak2 <= qrRankingThreshold) {
            ld->weightedJacobian=weightedJacobian;
            ld->permutation=permutation;
            ld->rank=k;
            ld->diagR=diagR;
            ld->jacNorm=jacNorm;
            ld->beta=beta;
            return ld;
        }
        int pk = permutation[nextColumn];
        permutation[nextColumn] = permutation[k];
        permutation[k] = pk;

        // choose alpha such that Hk.u = alpha ek
        double akk = weightedJacobian[k][pk];
        double alpha = (akk > 0) ? -sqrt(ak2) : sqrt(ak2);
        double betak = 1.0 / (ak2 - akk * alpha);
        beta[pk] = betak;

        // transform the current column
        diagR[pk] = alpha;
        weightedJacobian[k][pk] -= alpha;

        // transform the remaining columns
        int dk, j;
        for (dk = nC - 1 - k; dk > 0; --dk) {
            double gamma = 0;
            for (j = k; j < nR; ++j) {
                gamma += weightedJacobian[j][pk] * weightedJacobian[j][permutation[k + dk]];
            }
            gamma *= betak;
            for (j = k; j < nR; ++j) {
                weightedJacobian[j][permutation[k + dk]] -= gamma * weightedJacobian[j][pk];
            }
        }
    }

    ld->weightedJacobian=weightedJacobian;
    ld->permutation=permutation;
    ld->rank=solvedCols;
    ld->diagR=diagR;
    ld->jacNorm=jacNorm;
    ld->beta=beta;
    return ld;
}

void free_LeastSquaresInternalData(LeastSquaresInternalData *ld){
    int i;
    for(i=0;i<ld->n_data;i++) ld->weightedJacobian[i];
    free(ld->weightedJacobian);
    free(ld->permutation);
    free(ld->diagR);
    free(ld->jacNorm);
    free(ld->beta);
    free(ld);
}

int leastSquares_compare_xs(char *str1, char *str2){
    double d1, d2;

    int i=-1;
    while(str1[++i]!='\t');
    str1[i]='\0';
    d1=atof(str1);
    str1[i]='\t';

    i=-1;
    while(str2[++i]!='\t');
    str2[i]='\0';
    d2=atof(str2);
    str2[i]='\t';

    if(d1>d2) return 1;
    if(d1<d2) return -1;
    return 0;
}

void leastSquares_qTy(double* y, LeastSquaresInternalData *internalData){
    double** weightedJacobian = internalData->weightedJacobian;
    int* permutation = internalData->permutation;
    double* beta = internalData->beta;

    int nR = internalData->n_data;  //weightedJacobian.length;
    int nC = internalData->n_par;   //weightedJacobian[0].length;

    int i, j, k;

    for (k = 0; k < nC; ++k) {
        int pk = permutation[k];
        double gamma = 0;
        for (i = k; i < nR; ++i) {
            gamma += weightedJacobian[i][pk] * y[i];
        }
        gamma *= beta[pk];
        for (i = k; i < nR; ++i) {
            y[i] -= gamma * weightedJacobian[i][pk];
        }
    }
}

double leastSquares_determineLMParameter(double* qy, double delta, double* diag, LeastSquaresInternalData *internalData, int solvedCols, double* work1, double* work2, double* work3, double* lmDir, double lmPar){
    double** weightedJacobian = internalData->weightedJacobian;
    int* permutation = internalData->permutation;
    int rank = internalData->rank;
    double* diagR = internalData->diagR;

    double SAFE_MIN=2.2250738585072014E-308;

    int nC = internalData->n_par; // weightedJacobian[0].length;

    int i, j, k;

    // compute and store in x the gauss-newton direction, if the
    // jacobian is rank-deficient, obtain a least squares solution
    for (j = 0; j < rank; ++j) {
        lmDir[permutation[j]] = qy[j];
    }

    for (j = rank; j < nC; ++j) {
        lmDir[permutation[j]] = 0;
    }

    for (k = rank-1; k >= 0; --k) {
        int pk = permutation[k];
        double ypk = lmDir[pk] / diagR[pk];
        for (i = 0; i < k; ++i) {
            lmDir[permutation[i]] -= ypk * weightedJacobian[i][pk];
        }
        lmDir[pk] = ypk;
    }

    // evaluate the function at the origin, and test
    // for acceptance of the Gauss-Newton direction
    double dxNorm = 0;
    for (j = 0; j < solvedCols; ++j) {
        int pj = permutation[j];
        double s = diag[pj] * lmDir[pj];
        work1[pj] = s;
        dxNorm += s * s;
    }
    dxNorm = sqrt(dxNorm);
    double fp = dxNorm - delta;
    if (fp <= 0.1 * delta) {
        lmPar = 0;
        return lmPar;
    }

    // if the jacobian is not rank deficient, the Newton step provides
    // a lower bound, parl, for the zero of the function,
    // otherwise set this bound to zero
    double sum2;
    double parl = 0;
    if (rank == solvedCols) {
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            work1[pj] *= diag[pj] / dxNorm;
        }
        sum2 = 0;
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            double sum = 0;
            for (i = 0; i < j; ++i) {
                sum += weightedJacobian[i][pj] * work1[permutation[i]];
            }
            double s = (work1[pj] - sum) / diagR[pj];
            work1[pj] = s;
            sum2 += s * s;
        }
        parl = fp / (delta * sum2);
    }

    // calculate an upper bound, paru, for the zero of the function
    sum2 = 0;
    for (j = 0; j < solvedCols; ++j) {
        int pj = permutation[j];
        double sum = 0;
        for (i = 0; i <= j; ++i) {
            sum += weightedJacobian[i][pj] * qy[i];
        }
        sum /= diag[pj];
        sum2 += sum * sum;
    }
    double gNorm = sqrt(sum2);
    double paru = gNorm / delta;
    if (paru == 0) {
        paru = SAFE_MIN / min(delta, 0.1);
    }

    // if the input par lies outside of the interval (parl,paru),
    // set par to the closer endpoint
    lmPar = min(paru, max(lmPar, parl));
    if (lmPar == 0) {
        lmPar = gNorm / dxNorm;
    }

    int countdown;
    for (countdown = 10; countdown >= 0; --countdown) {

        // evaluate the function at the current value of lmPar
        if (lmPar == 0) {
            lmPar = max(SAFE_MIN, 0.001 * paru);
        }
        double sPar = sqrt(lmPar);
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            work1[pj] = sPar * diag[pj];
        }
        leastSquares_determineLMDirection(qy, work1, work2, internalData, solvedCols, work3, lmDir);

        dxNorm = 0;
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            double s = diag[pj] * lmDir[pj];
            work3[pj] = s;
            dxNorm += s * s;
        }
        dxNorm = sqrt(dxNorm);
        double previousFP = fp;
        fp = dxNorm - delta;

        // if the function is small enough, accept the current value
        // of lmPar, also test for the exceptional cases where parl is zero
        if (fabs(fp) <= 0.1 * delta ||
            (parl == 0 &&
             fp <= previousFP &&
             previousFP < 0)) {
            return lmPar;
        }

        // compute the Newton correction
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            work1[pj] = work3[pj] * diag[pj] / dxNorm;
        }
        for (j = 0; j < solvedCols; ++j) {
            int pj = permutation[j];
            work1[pj] /= work2[j];
            double tmp = work1[pj];
            for (i = j + 1; i < solvedCols; ++i) {
                work1[permutation[i]] -= weightedJacobian[i][pj] * tmp;
            }
        }
        sum2 = 0;
        for (j = 0; j < solvedCols; ++j) {
            double s = work1[permutation[j]];
            sum2 += s * s;
        }
        double correction = fp / (delta * sum2);

        // depending on the sign of the function, update parl or paru.
        if (fp > 0) {
            parl = max(parl, lmPar);
        } else if (fp < 0) {
            paru = min(paru, lmPar);
        }

        // compute an improved estimate for lmPar
        lmPar = max(parl, lmPar + correction);
    }

    return lmPar;
}

void leastSquares_determineLMDirection(double* qy, double* diag, double* lmDiag, LeastSquaresInternalData *internalData, int solvedCols, double* work, double* lmDir){
    int* permutation = internalData->permutation;
    double** weightedJacobian = internalData->weightedJacobian;
    double* diagR = internalData->diagR;

    int i, j, k;

    int nC= internalData->n_par;

    // copy R and Qty to preserve input and initialize s
    //  in particular, save the diagonal elements of R in lmDir
    for (j = 0; j < solvedCols; ++j) {
        int pj = permutation[j];
        for (i = j + 1; i < solvedCols; ++i) {
            weightedJacobian[i][pj] = weightedJacobian[j][permutation[i]];
        }
        lmDir[j] = diagR[pj];
        work[j]  = qy[j];
    }

    // eliminate the diagonal matrix d using a Givens rotation
    for (j = 0; j < solvedCols; ++j) {

        // prepare the row of d to be eliminated, locating the
        // diagonal element using p from the Q.R. factorization
        int pj = permutation[j];
        double dpj = diag[pj];
        if (dpj != 0) {
            //Arrays.fill(lmDiag, j + 1, lmDiag.length, 0);
            int tmp_index;
            for(tmp_index=j+1;tmp_index<nC;tmp_index++) lmDiag[tmp_index]=0.0;
        }
        lmDiag[j] = dpj;

        //  the transformations to eliminate the row of d
        // modify only a single element of Qty
        // beyond the first n, which is initially zero.
        double qtbpj = 0;
        for (k = j; k < solvedCols; ++k) {
            int pk = permutation[k];

            // determine a Givens rotation which eliminates the
            // appropriate element in the current row of d
            if (lmDiag[k] != 0) {

                double sin;
                double cos;
                double rkk = weightedJacobian[k][pk];
                if (fabs(rkk) < fabs(lmDiag[k])) {
                    double cotan = rkk / lmDiag[k];
                    sin   = 1.0 / sqrt(1.0 + cotan * cotan);
                    cos   = sin * cotan;
                } else {
                    double tan = lmDiag[k] / rkk;
                    cos = 1.0 / sqrt(1.0 + tan * tan);
                    sin = cos * tan;
                }

                // compute the modified diagonal element of R and
                // the modified element of (Qty,0)
                weightedJacobian[k][pk] = cos * rkk + sin * lmDiag[k];
                double temp = cos * work[k] + sin * qtbpj;
                qtbpj = -sin * work[k] + cos * qtbpj;
                work[k] = temp;

                // accumulate the tranformation in the row of s
                for (i = k + 1; i < solvedCols; ++i) {
                    double rik = weightedJacobian[i][pk];
                    double temp2 = cos * rik + sin * lmDiag[i];
                    lmDiag[i] = -sin * rik + cos * lmDiag[i];
                    weightedJacobian[i][pk] = temp2;
                }
            }
        }

        // store the diagonal element of s and restore
        // the corresponding diagonal element of R
        lmDiag[j] = weightedJacobian[j][permutation[j]];
        weightedJacobian[j][permutation[j]] = lmDir[j];
    }

    // solve the triangular system for z, if the system is
    // singular, then obtain a least squares solution
    int nSing = solvedCols;
    for (j = 0; j < solvedCols; ++j) {
        if ((lmDiag[j] == 0) && (nSing == solvedCols)) {
            nSing = j;
        }
        if (nSing < solvedCols) {
            work[j] = 0;
        }
    }
    if (nSing > 0) {
        for (j = nSing - 1; j >= 0; --j) {
            int pj = permutation[j];
            double sum = 0;
            for (i = j + 1; i < nSing; ++i) {
                sum += weightedJacobian[i][pj] * work[i];
            }
            work[j] = (work[j] - sum) / lmDiag[j];
        }
    }

    // permute the components of z back to components of lmDir
    for (j = 0; j < nC; ++j) {
        lmDir[permutation[j]] = work[j];
    }
}

//=========================================================== optimize Differential Evolution (DE)

double optimize_DE_min(int method_type, int npar, double *x, void *data, double (*func)(int, double*, void*), double *lowbound, double *upbound){
    int   i, j, L, n;      /* counting variables                 */
    int   r1, r2, r3, r4;  /* placeholders for random indexes    */
    int   r5;              /* placeholders for random indexes    */
    int   D;               /* Dimension of parameter vector      */
    int   NP;              /* number of population members       */
    int   imin;            /* index to member with lowest energy */
    int   strategy;        /* choice parameter for screen output */
    int   gen, genmax;
    double trial_cost;      /* buffer variable                    */
    double cvar;            /* computes the cost variance         */
    double cmean;           /* mean cost                          */
    double F,CR;            /* control variables of DE            */
    double cmin;            /* help variables                     */

//    char  strat[20][24];   /* strategy-indicator                 */
//    strcpy(strat[1], "DE/best/1/exp          ");
//    strcpy(strat[2], "DE/rand/1/exp          ");
//    strcpy(strat[3], "DE/rand-to-best/1/exp  ");
//    strcpy(strat[4], "DE/best/2/exp          ");
//    strcpy(strat[5], "DE/rand/2/exp          ");
//    strcpy(strat[6], "DE/best/1/bin          ");
//    strcpy(strat[7], "DE/rand/1/bin          ");
//    strcpy(strat[8], "DE/rand-to-best/1/bin  ");
//    strcpy(strat[9], "DE/best/2/bin          ");
//    strcpy(strat[10], "DE/rand/2/bin          ");

    strategy=method_type;   /*---choice of strategy-----------------*/
    genmax=10000;    /*---maximum number of generations------*/
    D=npar;            /*---number of parameters---------------*/
    NP=npar*10;        /*---population size.-------------------*/
    F=0.9;          /*---weight factor----------------------*/
    CR=1.0;         /*---crossing over factor---------------*/

    MyRand *myRand=new_my_rand1();
    double **c= my_new(NP, sizeof(double *));
    double **d= my_new(NP, sizeof(double *));
    for(i=0;i<NP;i++){
        c[i]= my_new(D, sizeof(double));
        d[i]= my_new(D, sizeof(double));
    }
    double **pold;
    double **pnew;
    double **pswap;
    double *tmp= my_new(D, sizeof(double));
    double *best= my_new(D, sizeof(double));
    double *bestit= my_new(D, sizeof(double));
    double *cost= my_new(NP, sizeof(double));

    for (i=0; i<NP; i++)
    {
        for (j=0; j<D; j++) /* spread initial population members */
        {
            c[i][j] = lowbound[j] + my_rand_next_double(myRand)*(upbound[j] - lowbound[j]);
        }
        cost[i] = func(D, c[i], data); /* obj. funct. value */
    }
    cmin = cost[0];
    imin = 0;
    for (i=1; i<NP; i++)
    {
        if (cost[i]<cmin)
        {
            cmin = cost[i];
            imin = i;
        }
    }

    memcpy(best, c[imin], D*sizeof(double));
    memcpy(bestit, c[imin], D*sizeof(double));

    pold = c; /* old population (generation G)   */
    pnew = d; /* new population (generation G+1) */

    gen = 0;                          /* generation counter reset */
    while ((gen < genmax) /*&& (kbhit() == 0)*/) /* remove comments if conio.h */
    {                                            /* is accepted by compiler    */
        gen++;
        imin = 0;

        for (i=0; i<NP; i++)         /* Start of loop through ensemble  */
        {
            do                        /* Pick a random population member */
            {                         /* Endless loop for NP < 2 !!!     */
                r1 = (int)(my_rand_next_double(myRand)*NP);
            }while(r1==i);

            do                        /* Pick a random population member */
            {                         /* Endless loop for NP < 3 !!!     */
                r2 = (int)(my_rand_next_double(myRand)*NP);
            }while((r2==i) || (r2==r1));

            do                        /* Pick a random population member */
            {                         /* Endless loop for NP < 4 !!!     */
                r3 = (int)(my_rand_next_double(myRand)*NP);
            }while((r3==i) || (r3==r1) || (r3==r2));

            do                        /* Pick a random population member */
            {                         /* Endless loop for NP < 5 !!!     */
                r4 = (int)(my_rand_next_double(myRand)*NP);
            }while((r4==i) || (r4==r1) || (r4==r2) || (r4==r3));

            do                        /* Pick a random population member */
            {                         /* Endless loop for NP < 6 !!!     */
                r5 = (int)(my_rand_next_double(myRand)*NP);
            }while((r5==i) || (r5==r1) || (r5==r2) || (r5==r3) || (r5==r4));

            if (strategy == 1) /* strategy DE0 (not in our paper) */
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                L = 0;
                do
                {
                    double v_new=bestit[n] + F * (pold[r2][n] - pold[r3][n]);
                    if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;

                    n = (n+1)%D;
                    L++;
                }while((my_rand_next_double(myRand) < CR) && (L < D));
            }
/*-------DE/rand/1/exp-------------------------------------------------------------------*/
/*-------This is one of my favourite strategies. It works especially well when the-------*/
/*-------"bestit[]"-schemes experience misconvergence. Try e.g. F=0.7 and CR=0.5---------*/
/*-------as a first guess.---------------------------------------------------------------*/
            else if (strategy == 2) /* strategy DE1 in the techreport */
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                L = 0;
                do
                {
                    double v_new=pold[r1][n] + F * (pold[r2][n] - pold[r3][n]);
                    if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;

                    n = (n+1)%D;
                    L++;
                }while((my_rand_next_double(myRand) < CR) && (L < D));
            }
/*-------DE/rand-to-best/1/exp-----------------------------------------------------------*/
/*-------This strategy seems to be one of the best strategies. Try F=0.85 and CR=1.------*/
/*-------If you get misconvergence try to increase NP. If this doesn't help you----------*/
/*-------should play around with all three control variables.----------------------------*/
            else if (strategy == 3) /* similiar to DE2 but generally better */
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                L = 0;
                do
                {
                    double v_new=tmp[n] + F * (bestit[n] - tmp[n]) + F * (pold[r1][n] - pold[r2][n]);
                    if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;

                    n = (n+1)%D;
                    L++;
                }while((my_rand_next_double(myRand) < CR) && (L < D));
            }
/*-------DE/best/2/exp is another powerful strategy worth trying--------------------------*/
            else if (strategy == 4)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                L = 0;
                do
                {
                    double v_new=bestit[n] + (pold[r1][n] + pold[r2][n] - pold[r3][n] - pold[r4][n]) * F;
                    if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;

                    n = (n+1)%D;
                    L++;
                }while((my_rand_next_double(myRand) < CR) && (L < D));
            }
/*-------DE/rand/2/exp seems to be a robust optimizer for many functions-------------------*/
            else if (strategy == 5)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                L = 0;
                do
                {
                    double v_new=pold[r5][n] + (pold[r1][n] + pold[r2][n] - pold[r3][n] - pold[r4][n]) * F;
                    if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;

                    n = (n+1)%D;
                    L++;
                }while((my_rand_next_double(myRand) < CR) && (L < D));
            }

/*=======Essentially same strategies but BINOMIAL CROSSOVER===============================*/

/*-------DE/best/1/bin--------------------------------------------------------------------*/
            else if (strategy == 6)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                for (L=0; L<D; L++) /* perform D binomial trials */
                {
                    if ((my_rand_next_double(myRand) < CR) || L == (D-1)) /* change at least one parameter */
                    {
                        double v_new=bestit[n] + F * (pold[r2][n] - pold[r3][n]);
                        if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;
                    }
                    n = (n+1)%D;
                }
            }
/*-------DE/rand/1/bin-------------------------------------------------------------------*/
            else if (strategy == 7)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                for (L=0; L<D; L++) /* perform D binomial trials */
                {
                    if ((my_rand_next_double(myRand) < CR) || L == (D-1)) /* change at least one parameter */
                    {
                        double v_new=pold[r1][n] + F * (pold[r2][n] - pold[r3][n]);
                        if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;
                    }
                    n = (n+1)%D;
                }
            }
/*-------DE/rand-to-best/1/bin-----------------------------------------------------------*/
            else if (strategy == 8)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                for (L=0; L<D; L++) /* perform D binomial trials */
                {
                    if ((my_rand_next_double(myRand) < CR) || L == (D-1)) /* change at least one parameter */
                    {
                        double v_new=tmp[n] + F * (bestit[n] - tmp[n]) + F * (pold[r1][n] - pold[r2][n]);
                        if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;
                    }
                    n = (n+1)%D;
                }
            }
/*-------DE/best/2/bin--------------------------------------------------------------------*/
            else if (strategy == 9)
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                for (L=0; L<D; L++) /* perform D binomial trials */
                {
                    if ((my_rand_next_double(myRand) < CR) || L == (D-1)) /* change at least one parameter */
                    {
                        double v_new=bestit[n] + (pold[r1][n] + pold[r2][n] - pold[r3][n] - pold[r4][n]) * F;
                        if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;
                    }
                    n = (n+1)%D;
                }
            }
/*-------DE/rand/2/bin--------------------------------------------------------------------*/
            else
            {
                memcpy(tmp, pold[i], D*sizeof(double));  //assignd(D,tmp,(*pold)[i]);
                n = (int)(my_rand_next_double(myRand)*D);
                for (L=0; L<D; L++) /* perform D binomial trials */
                {
                    if ((my_rand_next_double(myRand) < CR) || L == (D-1)) /* change at least one parameter */
                    {
                        double v_new=pold[r5][n] + (pold[r1][n] + pold[r2][n] - pold[r3][n] - pold[r4][n]) * F;
                        if(v_new>=lowbound[n] && v_new<=upbound[n]) tmp[n]=v_new;
                    }
                    n = (n+1)%D;
                }
            }


/*=======Trial mutation now in tmp[]. Test how good this choice really was.==================*/

            trial_cost = func(D, tmp, data);  // evaluate(D,tmp,&nfeval);  /* Evaluate new vector in tmp[] */

            if (trial_cost <= cost[i])   /* improved objective function value ? */
            {
                cost[i]=trial_cost;
                memcpy(pnew[i], tmp, D*sizeof(double));  //assignd(D,(*pnew)[i],tmp);
                if (trial_cost<cmin)          /* Was this a new minimum? */
                {                               /* if so...*/
                    cmin=trial_cost;           /* reset cmin to new low...*/
                    imin=i;
                    memcpy(best, tmp, D*sizeof(double));    //assignd(D,best,tmp);
                }
            }
            else
            {
                memcpy(pnew[i], pold[i], D*sizeof(double));   //assignd(D,(*pnew)[i],(*pold)[i]); /* replace target with old value */
            }
        }   /* End mutation loop through pop. */

        memcpy(bestit, best, D*sizeof(double)); //assignd(D,bestit,best);  /* Save best population member of current iteration */

        /* swap population arrays. New generation becomes old one */

        pswap = pold;
        pold  = pnew;
        pnew  = pswap;

/*----Compute the energy variance (just for monitoring purposes)-----------*/

        cmean = 0.;          /* compute the mean value first */
        for (j=0; j<NP; j++)
        {
            cmean += cost[j];
        }
        cmean = cmean/NP;

        cvar = 0.;           /* now the variance              */
        for (j=0; j<NP; j++)
        {
            cvar += (cost[j] - cmean)*(cost[j] - cmean);
        }
        cvar = cvar/(NP-1);
    }

    for(i=0;i<D;i++) x[i]=best[i];

    free(myRand);
    for(i=0;i<NP;i++){
        free(c[i]);
        free(d[i]);
    }
    free(c);
    free(d);
    free(tmp);
    free(best);
    free(bestit);
    free(cost);

    return func(D, x, data);
}

//================ socket

#ifndef _WIN32

int is_valid_ip(const char *ip){
    if(!ip) return 0;
    //--
    int section=0;
    int dot=0;
    while(*ip){
        if(*ip == '.'){
            dot++;
            if(dot > 3) return 0;
            if(section<0 || section >255) return 0;
            section=0;
        }else if(*ip >= '0' && *ip <= '9'){
            section = section * 10 + *ip - '0';
        }else{
            return 0;
        }
        ip++;
    }
    if(section<0 || section >255) return 0;
    if(dot!=3) return 0;
    //--
    return 1;
}

int socket_listen(int port){
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);

    int listen_fd= socket(AF_INET, SOCK_STREAM, 0);
    int num=0;
    while(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1){
        num++;
        if(num>10) return -1;
    }
    num=0;
    while(listen(listen_fd, 10) == -1){
        num++;
        if(num>10) return -1;
    }
    fprintf(stdout, "Ready for Accept, listening(port=%d)...\n", port);

    return listen_fd;
}

int socket_accept(int listen_id, int timeout_seconds) {
    struct timeval timeout;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(listen_id, &readfds);
    timeout.tv_sec = timeout_seconds;  // 设置超时时间
    timeout.tv_usec = 0;

    int ready = select(listen_id + 1, &readfds, NULL, NULL, &timeout);
    if (ready == -1){   //失败
        fprintf(stdout, "accept failed!\n");
        return -1;
    }else if (ready == 0){  //超时
        fprintf(stdout, "accept timeout!\n");
        return 0;
    }else {  // 成功接受连接
        struct sockaddr_in client_addr;
        int struct_len = sizeof(struct sockaddr_in);
        int sock_fd = accept(listen_id, (struct sockaddr *) &client_addr, &struct_len);
        fprintf(stdout, "%s:%d accept!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        set_socket_buff_size(sock_fd);
        return sock_fd;
    }
}

int socket_connect(char *ip, int port){
    int sockfd,numbytes;
    char buf[BUFSIZ];
    struct sockaddr_in their_addr;

    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        fprintf(stderr, "Socket creation error\n");
        return -1;
    }
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(port);
    //--
    if(is_valid_ip(ip)) their_addr.sin_addr.s_addr=inet_addr(ip);
    else{
        fprintf(stderr, "ip=%s is invalid, exit!\n", ip);
        exit(0);
        /*
        struct hostent *hent=gethostbyname(ip);
        if(!hent){
            fprintf(stderr, "ip=%s is invalid, exit!\n", ip);
            exit(0);
        }
        char *tmpIp=inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));
        their_addr.sin_addr.s_addr=inet_addr(tmpIp);
        free(tmpIp);
        */
    }
    //--
    memset(&(their_addr.sin_zero), 0, 8);
    if(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1){
        fprintf(stderr, "%s:%d Connection failed\n", ip, port);
        return -1;
    }
    fprintf(stdout, "%s:%d connected!\n", ip, port);
    set_socket_buff_size(sockfd);

    return sockfd;
}

void set_socket_buff_size(int sock_id){
    unsigned int uiRcvBuf = 0;
    unsigned int uiNewRcvBuf = 0;
    int uiRcvBufLen = sizeof(uiRcvBuf);
    int type=getsockopt(sock_id, SOL_SOCKET, SO_SNDBUF,(char*)&uiRcvBuf, &uiRcvBufLen);
    if(type<0){
        mylog("obtain socket buff size error");
        return;
    }
    uiRcvBuf=socketBuffSize;
    type=setsockopt(sock_id, SOL_SOCKET, SO_SNDBUF,(char*)&uiRcvBuf, uiRcvBufLen);
    if(type<0){
        mylog("set socket buff size error");
        return;
    }
}

void socket_send_data(int socket_id, char *buff, int64_t size){
    int sendSize=send(socket_id, &size, 8, 0);
    if(sendSize<0){
        sprintf(loginfo, "send error1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }else if(sendSize==0){
        sprintf(loginfo, "socket disconnected1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }
    //--
    char log[1000];
    char *t_start=getTime();
    sprintf(log, "%s sending(%lld)..", t_start, size);
    while(size){
        int64_t len=size<socketBuffSize ? size:socketBuffSize;
        sendSize=send(socket_id, buff, (int)len, 0);
        if(sendSize<0){
            sprintf(loginfo, "send error2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }else if(sendSize==0){
            sprintf(loginfo, "socket disconnected2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }
        buff+=sendSize;
        size-=sendSize;
    }
    char *t_end=getTime();
    fprintf(stdout, "%s\n%s sent.\n", log, t_end);
    free(t_start);
    free(t_end);
}

void socket_receive_data(int socket_id, char *buff, int64_t *resSize){
    int64_t size;
    int receivedSize=recv(socket_id, &size, 8, 0);
    if(receivedSize<0){
        sprintf(loginfo, "receive error1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }else if(receivedSize==0){
        sprintf(loginfo, "socket disconnected1(socket_id=%d)", socket_id);mylog(loginfo);
        exit(0);
    }
    *(resSize)=size;
    //--
    char log[1000];
    char *t_start=getTime();
    sprintf(log, "%s receiving..", t_start);
    while(size){
        int64_t len=size<socketBuffSize ? size:socketBuffSize;
        receivedSize=recv(socket_id, buff, (int)len, 0);
        if(receivedSize<0){
            sprintf(loginfo, "receive error2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }else if(receivedSize==0){
            sprintf(loginfo, "socket disconnected2(socket_id=%d)", socket_id);mylog(loginfo);
            exit(0);
        }
        buff+=receivedSize;
        size-=receivedSize;
    }
    char *t_end=getTime();
    fprintf(stdout, "%s\n%s received(%lld).\n", log, t_end, *(resSize));
    free(t_start);
    free(t_end);
}

#endif // _WIN32

//===========================================================

static inline char *util_str_deep_copy(char *str){int len=strlen(str)+1;char *res=(char *)calloc(len, sizeof(char));memcpy(res, str, len-1);return res;}

static inline int32_t hash_int(int key){int32_t h = (int32_t)key;h ^= (h >> 20) ^ (h >> 12);return h ^ (h >> 7) ^ (h >> 4);}

static inline int32_t hash_long(int64_t key){int32_t h=(int32_t)(key ^ (key >> 32));h ^= (h >> 20) ^ (h >> 12);return h ^ (h >> 7) ^ (h >> 4);}

static inline int32_t hash_double(double d){int64_t *l=(int64_t *)(&d);return hash_long(*(l));}

int32_t hash_str(char *key){int32_t i=0, h=0;while(key[i]) h=31*h+key[i++];return h;}

//=============== string_builder

inline SBuilder *new_s_builder(int64_t capacity){SBuilder *res=(SBuilder *)malloc(sizeof(SBuilder));res->tableSize=capacity;res->size=0;res->str=(char *)calloc(capacity, sizeof(char));return res;}

SBuilder *s_builder_deep_copy(SBuilder *sb){
    SBuilder *res=(SBuilder *)malloc(sizeof(SBuilder));
    res->tableSize=sb->tableSize;
    res->size=sb->size;
    res->str=(char *)malloc((sb->tableSize)*sizeof(char));
    int i;
    for(i=0;i<sb->size;i++){
        res->str[i]=sb->str[i];
    }
    res->str[i]='\0';
    return res;
}

inline void s_builder_add_char(SBuilder *sb, char c){s_builder_ensure(sb, sb->size+2);sb->str[sb->size++]=c;sb->str[sb->size]='\0';}

inline void s_builder_add_int(SBuilder *sb, int64_t v){char str[20];int len=sprintf(str, "%lld", v);s_builder_ensure(sb, sb->size+len+1);memcpy(sb->str+sb->size, str, len*sizeof(char));sb->size+=len;sb->str[sb->size]='\0';}

inline void s_builder_add_double(SBuilder *sb, double v){char str[100];int len=sprintf(str, "%f", v);s_builder_ensure(sb, sb->size+len+1);memcpy(sb->str+sb->size, str, len*sizeof(char));sb->size+=len;sb->str[sb->size]='\0';}

inline void s_builder_add_str(SBuilder *sb, char *str){int len=strlen(str);s_builder_ensure(sb, sb->size+len+1);memcpy(sb->str+sb->size, str, len*sizeof(char));sb->size+=len;sb->str[sb->size]='\0';}

inline void s_builder_add_str_with_len(SBuilder *sb, char *str, int len){s_builder_ensure(sb, sb->size+len+1);memcpy(sb->str+sb->size, str, len*sizeof(char));sb->size+=len;sb->str[sb->size]='\0';}

inline void s_builder_add_str_with_len2(SBuilder *sb, char *str, int len){sb->tableSize=sb->size+len+1;sb->str=(char *)realloc(sb->str, sb->tableSize*sizeof(char));memcpy(sb->str+sb->size, str, len*sizeof(char));sb->size+=len;sb->str[sb->size]='\0';}

inline void s_builder_compress(SBuilder *sb){if(!sb) return;sb->tableSize=sb->size+1;sb->str=realloc(sb->str, sb->tableSize);}

inline void s_builder_ensure(SBuilder *sb, int64_t minCapacity){if(minCapacity>sb->tableSize){int64_t newCapacity=minCapacity*3/2+1;sb->tableSize=newCapacity;sb->str=(char *)realloc(sb->str, newCapacity*sizeof(char));}}

inline void free_s_builder(SBuilder *sb){free(sb->str);free(sb);}

//=============== arrayList_int

inline AList_i *new_alist_i(int64_t initialCapacity){AList_i *res=(AList_i *)malloc(sizeof(AList_i));res->tableSize=initialCapacity;res->size=0;res->elementData=(int *)calloc(initialCapacity, sizeof(int));return res;}

inline AList_i *alist_i_deep_copy(AList_i *list){AList_i *res=(AList_i *)malloc(sizeof(AList_i));res->tableSize=list->tableSize;res->size=list->size;res->elementData=(int *)malloc(list->tableSize*sizeof(int));memcpy(res->elementData, list->elementData, list->size*sizeof(int));return res;}

inline void alist_i_add(AList_i *list, int value){alist_i_ensure(list, list->size+1);list->elementData[list->size++]=value;}

static inline void alist_i_ensure(AList_i *list, int64_t minCapacity){if(minCapacity>list->tableSize){int64_t newCapacity=list->tableSize*3/2+1;list->tableSize=newCapacity;list->elementData=(int *)realloc(list->elementData, newCapacity*sizeof(int));}}

void inline free_alist_i(AList_i *list){free(list->elementData);free(list);}

//=============== arrayList_long

inline AList_l *new_alist_l(int64_t initialCapacity){AList_l *res=(AList_l *)malloc(sizeof(AList_l));res->tableSize=initialCapacity;res->size=0;res->elementData=(int64_t *)calloc(initialCapacity, sizeof(int64_t));return res;}

inline AList_l *alist_l_deep_copy(AList_l *list){AList_l *res=(AList_l *)malloc(sizeof(AList_l));res->tableSize=list->tableSize;res->size=list->size;res->elementData=(int64_t *)malloc(list->tableSize*sizeof(int64_t));memcpy(res->elementData, list->elementData, list->size*sizeof(int64_t));return res;}

inline void alist_l_add(AList_l *list, int64_t value){alist_l_ensure(list, list->size+1);list->elementData[list->size++]=value;}

static inline void alist_l_ensure(AList_l *list, int64_t minCapacity){if(minCapacity>list->tableSize){int64_t newCapacity=list->tableSize*3/2+1;list->tableSize=newCapacity;list->elementData=(int64_t *)realloc(list->elementData, newCapacity*sizeof(int64_t));}}

inline void free_alist_l(AList_l *list){free(list->elementData);free(list);}

//=============== arrayList_double

inline AList_d *new_alist_d(int64_t initialCapacity){AList_d *res=(AList_d *)malloc(sizeof(AList_d));res->tableSize=initialCapacity;res->size=0;res->elementData=(double *)calloc(initialCapacity, sizeof(double));return res;}

inline AList_d *alist_d_deep_copy(AList_d *list){AList_d *res=(AList_d *)malloc(sizeof(AList_d));res->tableSize=list->tableSize;res->size=list->size;res->elementData=(double *)malloc(list->tableSize*sizeof(double));memcpy(res->elementData, list->elementData, list->size*sizeof(double));return res;}

inline void alist_d_add(AList_d *list, double value){alist_d_ensure(list, list->size+1);list->elementData[list->size++]=value;}

static inline void alist_d_ensure(AList_d *list, int64_t minCapacity){if(minCapacity>list->tableSize){int64_t newCapacity=list->tableSize*3/2+1;list->tableSize=newCapacity;list->elementData=(double *)realloc(list->elementData, newCapacity*sizeof(double));}}

inline void free_alist_d(AList_d *list){free(list->elementData);free(list);}

//=============== linkedList_long

inline LList_l *new_llist_l(){return (LList_l *)calloc(1, sizeof(LList_l));}

void llist_l_add(LList_l *list, int64_t value){
    LListNode_l *n=(LListNode_l *)calloc(1, sizeof(LListNode_l));
    n->value=value;
    if(!list->first){
        list->first=n;
        list->last=n;
    }else{
        list->last->next=(int64_t)n;
        n->prev=(int64_t)list->last;
        list->last=n;
    }
    list->size++;
}

void llist_l_add_first(LList_l *list, int64_t value){
    LListNode_l *n=(LListNode_l *)calloc(1, sizeof(LListNode_l));
    n->value=value;
    if(!list->first){
        list->first=n;
        list->last=n;
    }else{
        list->first->prev=(int64_t)n;
        n->next=(int64_t)list->first;
        list->first=n;
    }
    list->size++;
}

LListNode_l *llist_l_get(LList_l *list, int i){
    if(list->size<=i) return NULL;

    int j;
    if(i<(list->size>>1)){
        LListNode_l *x=list->first;
        for(j=0;j<i;j++) x=(LListNode_l *)x->next;
        return x;
    }else{
        LListNode_l *x=list->last;
        for(j=list->size-1;j>i;j--) x=(LListNode_l *)x->prev;
        return x;
    }
}

void llist_l_remove(LList_l *list, LListNode_l *n){
    if(n->next) ((LListNode_l *)n->next)->prev=n->prev;
    else list->last=(LListNode_l *)n->prev;
    //--
    if(n->prev) ((LListNode_l *)n->prev)->next=n->next;
    else list->first=(LListNode_l *)n->next;
    //--
    list->size--;
    free(n);
}

void llist_l_remove2(LList_l *list, int64_t value){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        //--
        if(c->value==value) llist_l_remove(list, c);
    }
}

void llist_l_clear(LList_l *list){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        free(c);
    }
    list->size=0;
    list->first=NULL;
    list->last=NULL;
}

void free_llist_l(LList_l *list){
    LListNode_l *n=list->first;
    while(n){
        LListNode_l *c=n;
        n=(LListNode_l *)n->next;
        free(c);
    }
    free(list);
}

//=============== hash_long_double

static inline void free_entry_ld(Entry_ld *e){if(!e) return;free_entry_ld((Entry_ld *)e->next);free(e);}

inline Hash_ld *new_hash_ld(){return new_hash_ld2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_ld *new_hash_ld1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_ld2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_ld *new_hash_ld2(int32_t size, double load_factor){Hash_ld *res=malloc(sizeof(Hash_ld));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_ld *));res->pairs=new_llist_l();return res;}

Entry_ld *hash_ld_get(Hash_ld *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_ld *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ld_put(Hash_ld *hp, int64_t key, double value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ld *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ld));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ld_resize(hp, hp->tableSize*2);
}

static void hash_ld_resize(Hash_ld *hp, int32_t newCapacity){
    Entry_ld **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ld **newTable=(Entry_ld **)calloc(hp->tableSize, sizeof(Entry_ld *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ld *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ld *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ld_remove(Hash_ld *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ld *e=hp->table[i];
    Entry_ld *pre=NULL;

    for(;e!=NULL;e=(Entry_ld *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ld *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ld_clear(Hash_ld *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ld(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ld(Hash_ld *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ld(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_long

static inline void free_entry_dl(Entry_dl *e){if(!e) return;free_entry_dl((Entry_dl *)e->next);free(e);}

inline Hash_dl *new_hash_dl(){return new_hash_dl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_dl *new_hash_dl1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_dl2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_dl *new_hash_dl2(int32_t size, double load_factor){Hash_dl *res=malloc(sizeof(Hash_dl));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_dl *));res->pairs=new_llist_l();return res;}

Entry_dl *hash_dl_get(Hash_dl *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_dl *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_dl_put(Hash_dl *hp, double key, int64_t value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_dl));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_dl_resize(hp, hp->tableSize*2);
}

static void hash_dl_resize(Hash_dl *hp, int32_t newCapacity){
    Entry_dl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_dl **newTable=(Entry_dl **)calloc(hp->tableSize, sizeof(Entry_dl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_dl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_dl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_dl_remove(Hash_dl *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dl *e=hp->table[i];
    Entry_dl *pre=NULL;

    for(;e!=NULL;e=(Entry_dl *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_dl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_dl_clear(Hash_dl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_dl(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_dl(Hash_dl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_dl(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_int

static inline void free_entry_di(Entry_di *e){if(!e) return;free_entry_di((Entry_di *)e->next);free(e);}

inline Hash_di *new_hash_di(){return new_hash_di2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_di *new_hash_di1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_di2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_di *new_hash_di2(int32_t size, double load_factor){Hash_di *res=malloc(sizeof(Hash_di));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_di *));res->pairs=new_llist_l();return res;}

Entry_di *hash_di_get(Hash_di *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_di *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_di_put(Hash_di *hp, double key, int value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_di *e=hp->table[i];

    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_di));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_di_resize(hp, hp->tableSize*2);
}

static void hash_di_resize(Hash_di *hp, int32_t newCapacity){
    Entry_di **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_di **newTable=(Entry_di **)calloc(hp->tableSize, sizeof(Entry_di *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_di *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_di *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_di_remove(Hash_di *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_di *e=hp->table[i];
    Entry_di *pre=NULL;

    for(;e!=NULL;e=(Entry_di *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_di *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_di_clear(Hash_di *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_di(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_di(Hash_di *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_di(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_double_double

static inline void free_entry_dd(Entry_dd *e){if(!e) return;free_entry_dd((Entry_dd *)e->next);free(e);}

inline Hash_dd *new_hash_dd(){return new_hash_dd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_dd *new_hash_dd1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_dd2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_dd *new_hash_dd2(int32_t size, double load_factor){Hash_dd *res=malloc(sizeof(Hash_dd));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_dd *));res->pairs=new_llist_l();return res;}

Entry_dd *hash_dd_get(Hash_dd *hp, double key){
    int32_t tmpHash=hash_double(key);
    Entry_dd *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_dd_put(Hash_dd *hp, double key, double value){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_dd));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_dd_resize(hp, hp->tableSize*2);
}

static void hash_dd_resize(Hash_dd *hp, int32_t newCapacity){
    Entry_dd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_dd **newTable=(Entry_dd **)calloc(hp->tableSize, sizeof(Entry_dd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_dd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_dd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_dd_remove(Hash_dd *hp, double key){
    int32_t tmpHash=hash_double(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_dd *e=hp->table[i];
    Entry_dd *pre=NULL;

    for(;e!=NULL;e=(Entry_dd *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_dd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_dd_clear(Hash_dd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_dd(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_dd(Hash_dd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_dd(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_double

static inline void free_entry_id(Entry_id *e){if(!e) return;free_entry_id((Entry_id *)e->next);free(e);}

inline Hash_id *new_hash_id(){return new_hash_id2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_id *new_hash_id1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_id2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_id *new_hash_id2(int32_t size, double load_factor){Hash_id *res=malloc(sizeof(Hash_id));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_id *));res->pairs=new_llist_l();return res;}

Entry_id *hash_id_get(Hash_id *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_id *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_id_put(Hash_id *hp, int key, double value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_id *e=hp->table[i];

    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_id));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_id_resize(hp, hp->tableSize*2);
}

static void hash_id_resize(Hash_id *hp, int32_t newCapacity){
    Entry_id **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_id **newTable=(Entry_id **)calloc(hp->tableSize, sizeof(Entry_id *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_id *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_id *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_id_remove(Hash_id *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_id *e=hp->table[i];
    Entry_id *pre=NULL;

    for(;e!=NULL;e=(Entry_id *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_id *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_id_clear(Hash_id *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_id(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_id(Hash_id *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_id(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_long

static inline void free_entry_il(Entry_il *e){if(!e) return;free_entry_il((Entry_il *)e->next);free(e);}

inline Hash_il *new_hash_il(){return new_hash_il2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_il *new_hash_il1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_il2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_il *new_hash_il2(int32_t size, double load_factor){Hash_il *res=malloc(sizeof(Hash_il));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_il *));res->pairs=new_llist_l();return res;}

Entry_il *hash_il_get(Hash_il *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_il *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_il_put(Hash_il *hp, int key, int64_t value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_il *e=hp->table[i];

    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_il));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_il_resize(hp, hp->tableSize*2);
}

static void hash_il_resize(Hash_il *hp, int32_t newCapacity){
    Entry_il **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_il **newTable=(Entry_il **)calloc(hp->tableSize, sizeof(Entry_il *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_il *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_il *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_il_remove(Hash_il *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_il *e=hp->table[i];
    Entry_il *pre=NULL;

    for(;e!=NULL;e=(Entry_il *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_il *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_il_clear(Hash_il *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_il(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_il(Hash_il *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_il(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_long_int

static inline void free_entry_li(Entry_li *e){if(!e) return;free_entry_li((Entry_li *)e->next);free(e);}

inline Hash_li *new_hash_li(){return new_hash_li2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_li *new_hash_li1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_li2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_li *new_hash_li2(int32_t size, double load_factor){Hash_li *res=malloc(sizeof(Hash_li));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_li *));res->pairs=new_llist_l();return res;}

Entry_li *hash_li_get(Hash_li *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_li *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_li_put(Hash_li *hp, int64_t key, int value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_li *e=hp->table[i];

    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_li));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_li_resize(hp, hp->tableSize*2);
}

static void hash_li_resize(Hash_li *hp, int32_t newCapacity){
    Entry_li **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_li **newTable=(Entry_li **)calloc(hp->tableSize, sizeof(Entry_li *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_li *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_li *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_li_remove(Hash_li *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_li *e=hp->table[i];
    Entry_li *pre=NULL;

    for(;e!=NULL;e=(Entry_li *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_li *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_li_clear(Hash_li *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_li(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_li(Hash_li *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_li(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_long_long

static inline void free_entry_ll(Entry_ll *e){if(!e) return;free_entry_ll((Entry_ll *)e->next);free(e);}

inline Hash_ll *new_hash_ll(){return new_hash_ll2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_ll *new_hash_ll1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_ll2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_ll *new_hash_ll2(int32_t size, double load_factor){Hash_ll *res=malloc(sizeof(Hash_ll));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_ll *));res->pairs=new_llist_l();return res;}

Entry_ll *hash_ll_get(Hash_ll *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    Entry_ll *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ll_put(Hash_ll *hp, int64_t key, int64_t value){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ll *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ll));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ll_resize(hp, hp->tableSize*2);
}

static void hash_ll_resize(Hash_ll *hp, int32_t newCapacity){
    Entry_ll **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ll **newTable=(Entry_ll **)calloc(hp->tableSize, sizeof(Entry_ll *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ll *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ll *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ll_remove(Hash_ll *hp, int64_t key){
    int32_t tmpHash=hash_long(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ll *e=hp->table[i];
    Entry_ll *pre=NULL;

    for(;e!=NULL;e=(Entry_ll *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ll *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ll_clear(Hash_ll *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ll(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ll(Hash_ll *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ll(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_int_int

static inline void free_entry_ii(Entry_ii *e){if(!e) return;free_entry_ii((Entry_ii *)e->next);free(e);}

inline Hash_ii *new_hash_ii(){return new_hash_ii2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_ii *new_hash_ii1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_ii2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_ii *new_hash_ii2(int32_t size, double load_factor){Hash_ii *res=malloc(sizeof(Hash_ii));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_ii *));res->pairs=new_llist_l();return res;}

Entry_ii *hash_ii_get(Hash_ii *hp, int key){
    int32_t tmpHash=hash_int(key);
    Entry_ii *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            return e;
        }
    }
    return NULL;
}

void hash_ii_put(Hash_ii *hp, int key, int value){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ii *e=hp->table[i];

    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_ii));
    e->hash=tmpHash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_ii_resize(hp, hp->tableSize*2);
}

static void hash_ii_resize(Hash_ii *hp, int32_t newCapacity){
    Entry_ii **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_ii **newTable=(Entry_ii **)calloc(hp->tableSize, sizeof(Entry_ii *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_ii *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_ii *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_ii_remove(Hash_ii *hp, int key){
    int32_t tmpHash=hash_int(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_ii *e=hp->table[i];
    Entry_ii *pre=NULL;

    for(;e!=NULL;e=(Entry_ii *)e->next){
        if(e->hash==tmpHash && e->key==key){
            if(pre==NULL) hp->table[i]=(Entry_ii *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_ii_clear(Hash_ii *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_ii(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_ii(Hash_ii *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_ii(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_double

static inline void free_entry_sd(Entry_sd *e){if(!e) return;free_entry_sd((Entry_sd *)e->next);free(e->key);free(e);}

inline Hash_sd *new_hash_sd(){return new_hash_sd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_sd *new_hash_sd1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_sd2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_sd *new_hash_sd2(int32_t size, double load_factor){Hash_sd *res=malloc(sizeof(Hash_sd));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_sd *));res->pairs=new_llist_l();return res;}

Entry_sd *hash_sd_get(Hash_sd *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_sd *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_sd_put(Hash_sd *hp, char *key, double value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_sd));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_sd_resize(hp, hp->tableSize*2);
}

static void hash_sd_resize(Hash_sd *hp, int32_t newCapacity){
    Entry_sd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_sd **newTable=(Entry_sd **)calloc(hp->tableSize, sizeof(Entry_sd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_sd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_sd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_sd_remove(Hash_sd *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sd *e=hp->table[i];
    Entry_sd *pre=NULL;

    for(;e!=NULL;e=(Entry_sd *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_sd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_sd_clear(Hash_sd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_sd(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_sd(Hash_sd *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_sd(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_int

static inline void free_entry_si(Entry_si *e){if(!e) return;free_entry_si((Entry_si *)e->next);free(e->key);free(e);}

inline Hash_si *new_hash_si(){return new_hash_si2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_si *new_hash_si1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_si2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_si *new_hash_si2(int32_t size, double load_factor){Hash_si *res=malloc(sizeof(Hash_si));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_si *));res->pairs=new_llist_l();return res;}

Entry_si *hash_si_get(Hash_si *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_si *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_si_put(Hash_si *hp, char *key, int value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_si *e=hp->table[i];

    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_si));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_si_resize(hp, hp->tableSize*2);
}

static void hash_si_resize(Hash_si *hp, int32_t newCapacity){
    Entry_si **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_si **newTable=(Entry_si **)calloc(hp->tableSize, sizeof(Entry_si *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_si *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_si *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_si_remove(Hash_si *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_si *e=hp->table[i];
    Entry_si *pre=NULL;

    for(;e!=NULL;e=(Entry_si *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_si *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_si_clear(Hash_si *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_si(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_si(Hash_si *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_si(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_str_long

static inline void free_entry_sl(Entry_sl *e){if(!e) return;free_entry_sl((Entry_sl *)e->next);free(e->key);free(e);}

inline Hash_sl *new_hash_sl(){return new_hash_sl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_sl *new_hash_sl1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_sl2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_sl *new_hash_sl2(int32_t size, double load_factor){Hash_sl *res=malloc(sizeof(Hash_sl));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_sl *));res->pairs=new_llist_l();return res;}

Entry_sl *hash_sl_get(Hash_sl *hp, char *key){
    int32_t tmpHash=hash_str(key);
    Entry_sl *e=hp->table[indexFor(tmpHash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_sl_put(Hash_sl *hp, char *key, int64_t value){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_sl));
    e->hash=tmpHash;
    e->key=util_str_deep_copy(key);
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_sl_resize(hp, hp->tableSize*2);
}

static void hash_sl_resize(Hash_sl *hp, int32_t newCapacity){
    Entry_sl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_sl **newTable=(Entry_sl **)calloc(hp->tableSize, sizeof(Entry_sl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_sl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_sl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_sl_remove(Hash_sl *hp, char *key){
    int32_t tmpHash=hash_str(key);
    int32_t i=indexFor(tmpHash, hp->tableSize);
    Entry_sl *e=hp->table[i];
    Entry_sl *pre=NULL;

    for(;e!=NULL;e=(Entry_sl *)e->next){
        if(e->hash==tmpHash && strcmp(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_sl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            free(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_sl_clear(Hash_sl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_sl(hp->table[i]);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_sl(Hash_sl *hp){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_sl(hp->table[i]);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_double

static inline void free_entry_vd(Entry_vd *e, void(*free_key)(void *key)){if(!e) return;free_entry_vd((Entry_vd *)e->next, free_key);if(free_key) free_key(e->key);free(e);}

inline Hash_vd *new_hash_vd(){return new_hash_vd2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_vd *new_hash_vd1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_vd2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_vd *new_hash_vd2(int32_t size, double load_factor){Hash_vd *res=malloc(sizeof(Hash_vd));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_vd *));res->pairs=new_llist_l();return res;}

Entry_vd *hash_vd_get(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vd *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vd_put(Hash_vd *hp, void *key, int hash, double value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vd *e=hp->table[i];

    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vd));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vd_resize(hp, hp->tableSize*2);
}

static void hash_vd_resize(Hash_vd *hp, int32_t newCapacity){
    Entry_vd **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vd **newTable=(Entry_vd **)calloc(hp->tableSize, sizeof(Entry_vd *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vd *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vd *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vd_remove(Hash_vd *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vd *e=hp->table[i];
    Entry_vd *pre=NULL;

    for(;e!=NULL;e=(Entry_vd *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vd *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vd_clear(Hash_vd *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vd(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vd(Hash_vd *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vd(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_int

static inline void free_entry_vi(Entry_vi *e, void(*free_key)(void *key)){if(!e) return;free_entry_vi((Entry_vi *)e->next, free_key);if(free_key) free_key(e->key);free(e);}

inline Hash_vi *new_hash_vi(){return new_hash_vi2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_vi *new_hash_vi1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_vi2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_vi *new_hash_vi2(int32_t size, double load_factor){Hash_vi *res=malloc(sizeof(Hash_vi));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_vi *));res->pairs=new_llist_l();return res;}

Entry_vi *hash_vi_get(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vi *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e;e=e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vi_put(Hash_vi *hp, void *key, int hash, int value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vi *e=hp->table[i];

    for(;e;e=e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vi));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vi_resize(hp, hp->tableSize*2);
}

static void hash_vi_resize(Hash_vi *hp, int32_t newCapacity){
    Entry_vi **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vi **newTable=(Entry_vi **)calloc(hp->tableSize, sizeof(Entry_vi *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vi *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vi *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vi_remove(Hash_vi *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vi *e=hp->table[i];
    Entry_vi *pre=NULL;

    for(;e!=NULL;e=(Entry_vi *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vi *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vi_clear(Hash_vi *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vi(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vi(Hash_vi *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vi(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== hash_void_long

static inline void free_entry_vl(Entry_vl *e, void(*free_key)(void *key)){if(!e) return;free_entry_vl((Entry_vl *)e->next, free_key);if(free_key) free_key(e->key);free(e);}

inline Hash_vl *new_hash_vl(){return new_hash_vl2(DEFAULT_INITIAL_CAPACITY, DEFAULT_LOAD_FACTOR);}

inline Hash_vl *new_hash_vl1(int32_t size){int n=size/DEFAULT_INITIAL_CAPACITY;size=DEFAULT_INITIAL_CAPACITY;while(n=n>>1) size<<1;return new_hash_vl2(size, DEFAULT_LOAD_FACTOR);}

inline Hash_vl *new_hash_vl2(int32_t size, double load_factor){Hash_vl *res=malloc(sizeof(Hash_vl));res->tableSize=size;res->size=0;res->threshold=(int32_t)(size*load_factor);res->table=calloc(size, sizeof(Entry_vl *));res->pairs=new_llist_l();return res;}

Entry_vl *hash_vl_get(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b)){
    Entry_vl *e=hp->table[indexFor(hash, hp->tableSize)];
    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            return e;
        }
    }
    return NULL;
}

void hash_vl_put(Hash_vl *hp, void *key, int hash, int64_t value, int(*compare)(void *a, void *b)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vl *e=hp->table[i];

    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            e->value=value;
            return;
        }
    }

    e=malloc(sizeof(Entry_vl));
    e->hash=hash;
    e->key=key;
    e->value=value;
    e->next=hp->table[i];
    hp->table[i]=e;

    llist_l_add(hp->pairs, (int64_t)e);
    e->node=hp->pairs->last;

    hp->size++;

    if(hp->size > hp->threshold) hash_vl_resize(hp, hp->tableSize*2);
}

static void hash_vl_resize(Hash_vl *hp, int32_t newCapacity){
    Entry_vl **oldTable=hp->table;
    int32_t oldSize=hp->tableSize;

    if(hp->tableSize == MAXIMUM_CAPACITY){
        hp->threshold = MAXIMUM_CAPACITY;
        return;
    }

    hp->tableSize=newCapacity;
    hp->threshold=(int32_t)(hp->tableSize*DEFAULT_LOAD_FACTOR);
    Entry_vl **newTable=(Entry_vl **)calloc(hp->tableSize, sizeof(Entry_vl *));

    int32_t i;
    for(i=0;i<oldSize;i++){
        Entry_vl *e=oldTable[i];
        if(!e) continue;
        if(!e->next) newTable[e->hash&(newCapacity-1)]=e;
        else{
            Entry_vl *loHead=NULL, *loTail=NULL, *hiHead=NULL, *hiTail=NULL, *next;
            do{
                next=e->next;
                if((e->hash & oldSize)==0){
                    if(!loTail) loHead=e;
                    else loTail->next=e;
                    loTail=e;
                }else{
                    if(!hiTail) hiHead=e;
                    else hiTail->next=e;
                    hiTail=e;
                }
            }while(e=e->next);
            if(loTail){
                loTail->next=NULL;
                newTable[i]=loHead;
            }
            if(hiTail){
                hiTail->next=NULL;
                newTable[i+oldSize]=hiHead;
            }
        }
    }
    free(oldTable);
    hp->table=newTable;
}

void hash_vl_remove(Hash_vl *hp, void *key, int hash, int(*compare)(void *a, void *b), void(*free_key)(void *key)){
    int32_t i=indexFor(hash, hp->tableSize);
    Entry_vl *e=hp->table[i];
    Entry_vl *pre=NULL;

    for(;e!=NULL;e=(Entry_vl *)e->next){
        if(e->hash==hash && compare(e->key, key)==0){
            if(pre==NULL) hp->table[i]=(Entry_vl *)e->next;
            else pre->next=e->next;

            llist_l_remove(hp->pairs, e->node);
            if(free_key) free_key(e->key);
            free(e);

            hp->size--;
            return;
        }
        pre=e;
    }
}

void hash_vl_clear(Hash_vl *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        if(!hp->table[i]) continue;
        free_entry_vl(hp->table[i], free_key);
        hp->table[i]=NULL;
    }
    llist_l_clear(hp->pairs);
    hp->size=0;
}

void free_hash_vl(Hash_vl *hp, void(*free_key)(void *key)){
    int32_t i;
    for(i=0;i<hp->tableSize;i++){
        free_entry_vl(hp->table[i], free_key);
    }
    free(hp->table);
    free_llist_l(hp->pairs);
    free(hp);
}

//=============== queue

Queue *new_queue(int size){
    Queue *res=(Queue *)malloc(sizeof(Queue));
    res->size=size;
    res->head=0;
    res->tail=0;
    res->count=0;
    res->array=(void **)calloc(size, sizeof(void *));
    res->is_finished=0;
    pthread_mutex_init(&(res->locker), NULL);
    pthread_cond_init(&(res->cond_put), NULL);
    pthread_cond_init(&(res->cond_get), NULL);
    return res;
}

void queue_put(Queue *q, void *data){
    pthread_mutex_lock(&(q->locker));
    if(q->is_finished){
        pthread_mutex_unlock(&(q->locker));
        fprintf(stderr, "Error: queue is finished, cannot put data!\n");
        exit(0);
    }
    while(q->count>=q->size){
        pthread_cond_wait(&(q->cond_get), &(q->locker));
    }
    q->count++;
    q->array[q->head]=data;
    q->head=(q->head+1)%(q->size);
    pthread_cond_signal(&(q->cond_put));
    pthread_mutex_unlock(&(q->locker));
}

inline void queue_set_finished(Queue *q){pthread_mutex_lock(&(q->locker));q->is_finished=1;pthread_cond_broadcast(&(q->cond_put));pthread_mutex_unlock(&(q->locker));}

void *queue_get(Queue *q){
    pthread_mutex_lock(&(q->locker));
    while(q->count<=0){
        if(q->is_finished){
            pthread_mutex_unlock(&(q->locker));
            return NULL;
        }else{
            pthread_cond_wait(&(q->cond_put), &(q->locker));
        }
    }
    q->count--;
    void *res=q->array[q->tail];
    q->array[q->tail]=NULL;
    q->tail=(q->tail+1)%(q->size);
    pthread_cond_signal(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
    return res;
}

inline void free_queue(Queue *q){free(q->array);pthread_mutex_destroy(&(q->locker));pthread_cond_destroy(&(q->cond_put));pthread_cond_destroy(&(q->cond_get));free(q);}

//=============== OrderQueue

OrderQueue *new_order_queue(int size){
    OrderQueue *res=(OrderQueue *)malloc(sizeof(OrderQueue));
    res->size=size;
    res->head=0;
    res->tail=0;
    res->count=0;
    res->index=0;
    res->array=(void **)calloc(size, sizeof(void *));
    res->is_finished=0;
    pthread_mutex_init(&(res->locker), NULL);
    pthread_cond_init(&(res->cond_put), NULL);
    pthread_cond_init(&(res->cond_get), NULL);
    return res;
}

void order_queue_put(OrderQueue *q, void *data, int index){
    pthread_mutex_lock(&(q->locker));
    if(q->is_finished){
        pthread_mutex_unlock(&(q->locker));
        fprintf(stderr, "Error: queue is finished, cannot put data!\n");
        exit(0);
    }
    while(index>q->index || q->count>=q->size){
        pthread_cond_wait(&(q->cond_get), &(q->locker));
    }
    q->count++;
    q->index++;
    q->array[q->head]=data;
    q->head=(q->head+1)%(q->size);
    pthread_cond_signal(&(q->cond_put));
    pthread_cond_broadcast(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
}

inline void order_queue_set_finished(OrderQueue *q){
    pthread_mutex_lock(&(q->locker));
    q->is_finished=1;
    pthread_mutex_unlock(&(q->locker));
}

void *order_queue_get(OrderQueue *q){
    pthread_mutex_lock(&(q->locker));
    while(q->count<=0){
        if(q->is_finished){
            pthread_mutex_unlock(&(q->locker));
            return NULL;
        }else{
            pthread_cond_wait(&(q->cond_put), &(q->locker));
        }
    }
    q->count--;
    void *res=q->array[q->tail];
    q->array[q->tail]=NULL;
    q->tail=(q->tail+1)%(q->size);
    pthread_cond_broadcast(&(q->cond_get));
    pthread_mutex_unlock(&(q->locker));
    return res;
}

inline void free_order_queue(OrderQueue *q){
    free(q->array);
    pthread_mutex_destroy(&(q->locker));
    pthread_cond_destroy(&(q->cond_put));
    pthread_cond_destroy(&(q->cond_get));
    free(q);
}

//=============== array_deque_long

inline ADeque_l *new_adeque_l(int64_t initialSize){
ADeque_l *q=(ADeque_l *)calloc(1, sizeof(ADeque_l));
q->elements=(int64_t *)calloc(initialSize, sizeof(int64_t));
q->l_elements=initialSize;
return q;
}

static void adeque_l_double_capacity(ADeque_l *q){
    int64_t p=q->head;
    int64_t n=q->l_elements;
    int64_t r=n-p;
    int64_t newCapacity=n<<2;
    if(newCapacity<0){
        fprintf(stderr, "Sorry, deque too big\n");
        exit(0);
    }
    int64_t *a=calloc(newCapacity, sizeof(int64_t));
    memcpy(a, q->elements+p, r*sizeof(int64_t));
    memcpy(a+r, q->elements, p*sizeof(int64_t));
    free(q->elements);
    q->elements=a;
    q->l_elements=newCapacity;
    q->head=0;
    q->tail=n;
}

inline void adeque_l_add_first(ADeque_l *q, int64_t value){
    int64_t *p=(int64_t *)malloc(sizeof(int64_t));
    *(p)=value;
    q->elements[q->head = (q->head - 1) & (q->l_elements-1)] = (int64_t)p;
    if(q->head==q->tail) adeque_l_double_capacity(q);
}

inline void adeque_l_add_last(ADeque_l *q, int64_t value){
    int64_t *p=(int64_t *)malloc(sizeof(int64_t));
    *(p)=value;
    q->elements[q->tail]=(int64_t)p;
    if ( (q->tail = (q->tail + 1) & (q->l_elements-1)) == q->head) adeque_l_double_capacity(q);
}

inline int64_t *adeque_l_poll_first(ADeque_l *q){
int64_t h=q->head;
int64_t *result=(int64_t *)(q->elements[h]);
if(!result) return NULL;
q->elements[h]=(int64_t)NULL;
q->head = (h + 1) & (q->l_elements - 1);
return result;
}

inline int64_t *adeque_l_poll_last(ADeque_l *q){
int64_t t=(q->tail-1)&(q->l_elements-1);
int64_t *result=(int64_t *)(q->elements[t]);
if(!result) return NULL;
q->elements[t]=(int64_t)NULL;
q->tail=t;
return result;
}

inline int64_t *adeque_l_peek_first(ADeque_l *q){
return (int64_t *)(q->elements[q->head]);
}

inline int64_t *adeque_l_peek_last(ADeque_l *q){
return (int64_t *)(q->elements[(q->tail-1)&(q->l_elements-1)]);
}

inline void adeque_l_add(ADeque_l *q, int64_t value){
    adeque_l_add_last(q, value);
}

inline int64_t *adeque_l_poll(ADeque_l *q){
return adeque_l_poll_first(q);
}

inline int64_t *adeque_l_peek(ADeque_l *q){
return adeque_l_peek_first(q);
}

void free_adeque_l(ADeque_l *q){
    int i;
    for(i=0;i<q->l_elements;i++){
        int64_t *p=(int64_t *)q->elements[i];
        if(p) free(p);
    }
    free(q->elements);
    free(q);
}

//=============== PriorityQueue

inline PriorityQueue *new_priority_queue(int capacity, int (*compare)(void *, void *)){
    PriorityQueue *q=malloc(sizeof(PriorityQueue));
    q->capacity=capacity;
    q->queue=(void **)calloc(capacity, sizeof(void *));
    q->compare=compare;
    q->size=0;
    return q;
}

int priority_queue_add(PriorityQueue *q, void *value){
    int i=q->size;
    if(i>=q->capacity){
        int oldCapacity=q->capacity;
        int newCapacity = oldCapacity + ((oldCapacity < 64) ? (oldCapacity + 2) : (oldCapacity >> 1));
        q->capacity=newCapacity;
        q->queue=realloc(q->queue, newCapacity*sizeof(void *));
        memset(q->queue+oldCapacity, 0, (newCapacity-oldCapacity)*sizeof(void *));
    }
    q->size=i+1;
    if(i==0) q->queue[0]=value;
    else{
        while(i>0){
            int parent=(i-1)>>1;
            void *e=q->queue[parent];
            if(q->compare(value, e)>=0) break;
            q->queue[i] =e;
            i=parent;
        }
        q->queue[i]=value;
    }
}

void *priority_queue_poll(PriorityQueue *q){
    if(q->size==0) return NULL;
    int s=--q->size;
    void *result=q->queue[0];
    void *x=q->queue[s];
    q->queue[s]=NULL;
    if(s!=0){
        int k=0;
        int half=q->size>>1;
        while(k<half) {
            int child=(k<<1)+1;
            void *c=q->queue[child];
            int right=child+1;
            if (right<q->size && q->compare(c, q->queue[right])>0) c=q->queue[child=right];
            if (q->compare(x, c)<=0) break;
            q->queue[k]=c;
            k=child;
        }
        q->queue[k] = x;
    }
    return result;
}

void free_priority_queue(PriorityQueue *q, void (*myfree)(void *)){
    if(myfree){
        int i;
        for(i=0;i<q->size;i++){
            void *x=q->queue[i];
            if(x) myfree(x);
        }
    }
    free(q->queue);
    free(q);
}

//=============== thread pool

pthread_t my_threads_array[50000];
int my_threads_array_index=0;

inline void go_run_thread(void *(*func)(void *arg), void *arg){
    int stat=pthread_create(my_threads_array+my_threads_array_index++, NULL, func, arg);
    if(stat!=0){
        fprintf(stderr, "create thread error, state=%d\n", stat);
        exit(0);
    }
}

void wg_wait_thread(){
    int i;
    for(i=0;i<my_threads_array_index;i++) pthread_join(my_threads_array[i], NULL);
    my_threads_array_index=0;
}

ThreadPool *new_thread_pool(int pool_thread_num){
    ThreadPool *p=(ThreadPool *)calloc(1, sizeof(ThreadPool));

    int i;

    p->thread_num=pool_thread_num;
    p->is_wait=calloc(pool_thread_num, sizeof(int));
    for(i=0;i<pool_thread_num;i++) p->is_wait[i]=1;
    //--
    pthread_mutex_init(&(p->locker), NULL);
    pthread_cond_init(&(p->cond), NULL);
    pthread_cond_init(&(p->cond2), NULL);
    //--
    p->threads=(pthread_t *)malloc(pool_thread_num*sizeof(pthread_t));
    for(i=0;i<pool_thread_num;i++){
        char *para=(char *)malloc(100*sizeof(char));
        sprintf(para, "%d\t%lld", i, p);
        int stat=pthread_create(p->threads+i, NULL, thread_routine, (void *)para);
        if(stat!=0){
            fprintf(stderr, "create thread error, state=%d\n", stat);
            free(para);
            exit(0);
        }
    }

    return p;
}

static void *thread_routine(void *args){
    char *para=(char *)args;

    int i=0;
    while(para[++i]!='\t');
    para[i]='\0';
    int threadIndex=atoi(para);
    ThreadPool *p=(ThreadPool *)atoll(para+i+1);
    para[i]='\t';
    free(para);

    while(1){
        pthread_mutex_lock(&(p->locker));
        if(p->is_wait[threadIndex]) pthread_cond_wait(&(p->cond), &(p->locker));
        if(p->is_finished){
            pthread_mutex_unlock(&(p->locker));
            break;
        }
        pthread_mutex_unlock(&(p->locker));

        while(1){
            ThreadWorker *w=NULL;
            pthread_mutex_lock(&(p->locker));
            w=p->head;
            if(w) p->head=w->next;
            pthread_mutex_unlock(&(p->locker));
            if(!w) break;
            (*(w->process))(w->arg);
            free(w);
        }

        pthread_mutex_lock(&(p->locker));
        p->is_wait[threadIndex]=1;
        p->fininshed_num++;
        if(p->fininshed_num==p->thread_num) pthread_cond_signal(&(p->cond2));
        pthread_mutex_unlock(&(p->locker));
    }
}

void thread_pool_add_worker(ThreadPool *p, void *(*func)(void *arg), void *arg){
    ThreadWorker *w=(ThreadWorker *)calloc(1, sizeof(ThreadWorker));
    w->process=func;
    w->arg=arg;
    w->next=p->head;
    p->head=w;
}

void thread_pool_invoke_all(ThreadPool *p){
    pthread_mutex_lock(&(p->locker));
    //--
    p->fininshed_num=0;
    memset(p->is_wait, 0, p->thread_num*sizeof(int));
    pthread_cond_broadcast(&(p->cond));
    pthread_cond_wait(&(p->cond2), &(p->locker));
    //--
    pthread_mutex_unlock(&(p->locker));
}

void free_thread_pool(ThreadPool *p){
    pthread_mutex_lock(&(p->locker));
    //--
    p->is_finished=1;
    p->fininshed_num=0;
    memset(p->is_wait, 0, p->thread_num*sizeof(int));
    pthread_cond_broadcast(&(p->cond));
    //--
    pthread_mutex_unlock(&(p->locker));
    //--
    int i;
    for(i=0;i<p->thread_num;i++) pthread_join(p->threads[i], NULL);
    //--
    pthread_mutex_destroy(&(p->locker));
    pthread_cond_destroy(&(p->cond));
    pthread_cond_destroy(&(p->cond2));
    free(p->is_wait);
    free(p->threads);
    free(p);
}

//===========================================================



