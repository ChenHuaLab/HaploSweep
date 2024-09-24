#include "iHS_soft.h"

long starts_thread[20000];
long ends_thread[20000];
//--
int tabs[1000000];
int64_t *all_processed_locus;
double *norm_min_value_array;
double norm_min_value;
int64_t *norm_talbe;
double *norm_uiHS_avarage;
double *norm_uiHS_variance;
double *norm_uRiHS1_avarage;
double *norm_uRiHS1_variance;
double *norm_uRiHS0_avarage;
double *norm_uRiHS0_variance;
double *norm_uiHSL_avarage;
double *norm_uiHSL_variance;
Hash_si *norm_freq_index;
AList_d *norm_all_freqs;

//====================================

void split_span_for_threads(int thread_num, long start, long end){
    long sp=(end-start)/(long)thread_num;
    long last=(end-start)%(long)thread_num;
    long s, e=start;
    int t;
    for(t=0;t<thread_num;t++){
        s=e;
        e=s+sp;
        if(t<last) e++;
        starts_thread[t]=s;
        ends_thread[t]=e;
    }
}

//====================================

int main(int argc, char **argv){
    argc--;
    argv++;
    if(argc<1) print_usage0();
    //--
    if(strcmp(argv[0], "calc")==0) main_process1(argc, argv);
    else if(strcmp(argv[0], "norm")==0) main_process2(argc, argv);
    else print_usage0();
}

void print_usage0(){
    printf("===========================\n");
    printf("Author:   ChiLianjiang\n");
    printf("E-mail:   chilianjiang@126.com\n");
    printf("Date:     2021-03-26\n");
    printf("Version:  1.0\n\n");
    printf("Usage: HaploSweep calc [arguments]\n");
    printf("Usage: HaploSweep norm [arguments]\n\n");
    exit(0);
}

//====================================

int main_process1(int argc, char **argv){
    load_parameters1(argc, argv++);

    mylog("starting...");

    int i, j;

    mylog("reading data...");
    if(str_ends(dataFile, ".hap")) read_hap();
    else if(str_ends(dataFile, ".vcf") || str_ends(dataFile, ".vcf.gz")) read_vcf();
    else if(str_ends(dataFile, ".tped")) read_tped();
    else{
        fprintf(stderr, "(%s) error format file\n", dataFile);
        exit(0);
    }
    group_size=n_sample<160 ? 0.2:0.1;
    sprintf(loginfo, "sample_num=%d\tlocs_num=%d", n_sample, n_locs);mylog(loginfo);

    results=my_new(n_locs, sizeof(Result));

    mylog("getting all process loucs...");
    all_processed_locus=calloc(thread_num, sizeof(int64_t));
    pool=new_thread_pool(thread_num);
    split_span_for_threads(thread_num, 0, (n_locs-2*window_radius));
    for(i=0;i<thread_num;i++) thread_pool_add_worker(pool, (void *(*)(void *))run_one1, (void *)i);
    thread_pool_invoke_all(pool);
    mylog("calculating all lucus...");
    for(i=0;i<thread_num;i++){
        AList_i *locus=(AList_i *)all_processed_locus[i];
        for(j=0;j<locus->size;j++) thread_pool_add_worker(pool, (void *(*)(void *))run_one2, (void *)locus->elementData[j]);
        free_alist_i(locus);
    }
    free(all_processed_locus);
    thread_pool_invoke_all(pool);
    free_thread_pool(pool);

    mylog("writing results into file...");
    if(outFile1) write_output1();
    if(outFile2) write_output2();

    mylog("finished!");
    return 0;
}

void load_parameters1(int argc, char **argv){
    argc--;
    argv++;

    if(argc<6) print_usage1();

    region_start=-1;
    region_end=-1;
    is_trunc_ok=0;
    trunc_rate=0.5;
    min_allele_frequency=0.05;
    ehh_min_value=0.05;
    scale_value=20000;
    max_gap=200000;
    dataFile=NULL;
    mapFile=NULL;
    window_radius=400;
    group_size=0.1;
    max_extend=1000000;
    thread_num=8;
    outFile1=NULL;
    outFile2=NULL;

    int i, j;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i], "-i")==0){
            i++;
            dataFile=argv[i];
        }else if(strcmp(argv[i], "-m")==0){
            i++;
            mapFile=argv[i];
        }else if(strcmp(argv[i], "-region")==0){
            i++;
            char *str=str_copy(argv[i]);
            j=0;
            while(str[++j]!='-');
            str[j]='\0';
            region_start=atoi(str);
            region_end=atoi(str+j+1);
            free(str);
        }else if(strcmp(argv[i], "-trunc-ok")==0){
            i++;
            is_trunc_ok=atoi(argv[i]);
        }else if(strcmp(argv[i], "-trunc")==0){
            i++;
            trunc_rate=atof(argv[i]);
        }else if(strcmp(argv[i], "-maf")==0){
            i++;
            min_allele_frequency=atof(argv[i]);
        }else if(strcmp(argv[i], "-cutoff")==0){
            i++;
            ehh_min_value=atof(argv[i]);
        }else if(strcmp(argv[i], "-gap-scale")==0){
            i++;
            scale_value=atoi(argv[i]);
        }else if(strcmp(argv[i], "-max-gap")==0){
            i++;
            max_gap=atoi(argv[i]);
        }else if(strcmp(argv[i], "-w")==0){
            i++;
            window_radius=atoi(argv[i]);
        }else if(strcmp(argv[i], "-r")==0){
            i++;
            group_size=atof(argv[i]);
        }else if(strcmp(argv[i], "-e")==0){
            i++;
            max_extend=atoi(argv[i]);
        }else if(strcmp(argv[i], "-t")==0){
            i++;
            thread_num=atoi(argv[i]);
        }else if(strcmp(argv[i], "-o1")==0){
            i++;
            outFile1=argv[i];
        }else if(strcmp(argv[i], "-o2")==0){
            i++;
            outFile2=argv[i];
        }else{
            fprintf(stderr, "error: %s is not included in parameters!\n", argv[i]);
            print_usage1();
        }
    }

    if(dataFile==NULL){
        fprintf(stderr, "-i is needed!\n");
        exit(0);
    }

    if(str_ends(dataFile, ".hap") || str_ends(dataFile, ".vcf") || str_ends(dataFile, ".vcf.gz")){
        if(mapFile==null){
            fprintf(stderr, "-m is needed!\n");
            exit(0);
        }
    }else if(!str_ends(dataFile, "tped")){
        fprintf(stderr, "data file's postfix is error!\n");
        exit(0);
    }

    if(outFile1==NULL && outFile2==NULL){
        fprintf(stderr, "-o1 or -o2 is needed!\n");
        exit(0);
    }

    if(region_start>region_end){
        fprintf(stderr, "error: region_start must be smaller than region_end!\n");
        exit(0);
    }

    if(thread_num<1){
        fprintf(stderr, "thread number must be large than 0\n");
        exit(0);
    }
}

void print_usage1(){
    printf("===========================\n");
    printf("Author:   ChiLianjiang\n");
    printf("E-mail:   chilianjiang@126.com\n");
    printf("Date:     2021-03-26\n");
    printf("Version:  1.0\n\n");
    printf("Usage: HaploSweep calc [arguments]\n");
    printf("  input:\n");
    printf("    -i          input data file(.hap .vcf .vcf.gz .tped)\n");
    printf("    -m          input map distance file(for .tped, not needed)\n");
    printf("  options:\n");
    printf("    -w          (int)window radius(default:400)\n");
    //printf("    -r          (double)group size(default:0.1)\n");
    printf("    -e          (int)max extend distance(default:1000000)\n");
    printf("    -t          (int)thread number(default:8)\n");
    printf("    -region     (int)start(include)-end(include), 1-based(default:full region)\n");
    printf("    -trunc-ok   (int)1: retain the locus which not extend to maximum(default:0)\n");
    printf("    -trunc      (double)trunc rate, if <, discard this locus(default:0.5)\n");
    printf("    -maf        (double)min allele frequency(default:0.05)\n");
    printf("    -cutoff     (double)min ehh value(default:0.05)\n");
    printf("    -gap-scale  (int)gap scale(default:20000)\n");
    printf("    -max-gap    (int)max extend gap(default:200000)\n");
    printf("  output:\n");
    printf("    -o1         output file\n");
    printf("    -o2         output iHSL details file\n\n");
    exit(0);
}

//====================================

int main_process2(int argc, char **argv){
    load_parameters2(argc, argv++);

    mylog("starting...");

    int i, j;

    norm_freq_index= new_hash_si1(1<<10);
    norm_all_freqs= new_alist_d(1<<10);

    AList_l *files=get_all_calc_results_file();
    read_norm_data(files);
    free_alist_l(files);

    mylog("calculating...");
    norm_talbe=my_new(n_sample, sizeof(int64_t));
    norm_uiHS_avarage=my_new(n_sample, sizeof(double));
    norm_uiHS_variance=my_new(n_sample, sizeof(double));
    norm_uRiHS1_avarage=my_new(n_sample, sizeof(double));
    norm_uRiHS1_variance=my_new(n_sample, sizeof(double));
    norm_uRiHS0_avarage=my_new(n_sample, sizeof(double));
    norm_uRiHS0_variance=my_new(n_sample, sizeof(double));
    norm_uiHSL_avarage=my_new(n_sample, sizeof(double));
    norm_uiHSL_variance=my_new(n_sample, sizeof(double));

    for(i=0;i<l_norm_data;i++){
        NormData data=norm_data[i];
        AList_i *list=(AList_i *)norm_talbe[data.num_1];
        if(list==NULL) list=new_alist_i(16);
        alist_i_add(list, i);
        norm_talbe[data.num_1]=(int64_t)list;
    }

    ThreadPool *pool=new_thread_pool(thread_num);

    mylog("calculating all rates' average and variance...");
    for(i=0;i<n_sample;i++) thread_pool_add_worker(pool, (void *(*)(void *))calculate_all_average_variance_one, (void *)i);
    thread_pool_invoke_all(pool);

    mylog("calculating all locus...");
    split_span_for_threads(thread_num, 0, l_norm_data);
    for(i=0;i<thread_num;i++) thread_pool_add_worker(pool, (void *(*)(void *))norm_calculate_one, (void *)i);
    thread_pool_invoke_all(pool);

    free_thread_pool(pool);

    mylog("writing results into file...");
    GzStream *out=gz_stream_open(norm_output, "w");
    int len=sprintf(loginfo, "#locusID\tchr\tphysicalPos\t'1'_freq\tiHS\tRiHS1\tRiHS0\tiHSL\tRiHSL\tp(iHSL)\tp(RiHSL)\n");
    gz_write(out, loginfo, len);
    for(i=0;i<l_norm_data;i++){
        NormData data=norm_data[i];
        len=sprintf(loginfo, "%s\t%s\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%.3e\t%.3e\n", data.rs, data.chr, data.physical_distance, data.freq_1, data.iHS, data.RiHS1, data.RiHS0, data.iHSL, data.RiHSL, data.p_iHSL, data.p_RiHSL);
        gz_write(out, loginfo, len);
    }
    gz_stream_destory(out);

    mylog("finished!");
}

void load_parameters2(int argc, char **argv){
    argc--;
    argv++;

    if(argc<4) print_usage2();

    norm_input_dir=NULL;
    thread_num=8;
    norm_bin_size=0.01;
    norm_output=NULL;

    int i;
    for(i=0;i<argc;i++){
        if(strcmp(argv[i], "-dir")==0){
            i++;
            norm_input_dir=argv[i];
        }else if(strcmp(argv[i], "-t")==0){
            i++;
            thread_num=atoi(argv[i]);
        }else if(strcmp(argv[i], "-bin")==0){
            i++;
            norm_bin_size=atof(argv[i]);
        }else if(strcmp(argv[i], "-out")==0){
            i++;
            norm_output=argv[i];
        }else{
            fprintf(stderr, "error: %s is not included in parameters!\n", argv[i]);
            print_usage1();
        }
    }

    if(thread_num<1){
        fprintf(stderr, "-t should larger than 0!\n");
        exit(0);
    }

    if(norm_input_dir==NULL){
        fprintf(stderr, "-dir is needed!\n");
        exit(0);
    }

    if(norm_output==NULL){
        fprintf(stderr, "-out is needed!\n");
        exit(0);
    }
}

void print_usage2(){
    printf("===========================\n");
    printf("Author:   ChiLianjiang\n");
    printf("E-mail:   chilianjiang@126.com\n");
    printf("Date:     2021-03-26\n");
    printf("Version:  1.0\n\n");
    printf("Usage: HaploSweep norm [arguments]\n");
    printf("  input:\n");
    printf("    -dir     input directory\n");
    printf("  options:\n");
    printf("    -t       (int)thread number(default:8)\n");
    printf("    -bin     (double)bin size(default:0.01)\n");
    printf("  output:\n");
    printf("    -out     output file\n\n");
    exit(0);
}

//====================================

void read_hap(){
    int i, j, len;

    n_locs=0;
    n_sample=0;

    AList_l *list=new_alist_l(16);

    GzStream *gz1=gz_stream_open(dataFile, "r");
    SBuilder *sb=new_s_builder(1000000);
    while(gz_read_util(gz1, '\n', line, max_line_length, &len)){
        len=chmop_with_len(line, len);
        //--
        sb->size=0;
        sb->str[0]='\0';
        //--
        for(i=0;i<len;i++){
            char c=line[i];
            if(c==' ' || c=='\t') continue;
            if(c!='0' && c!='1'){
                fprintf(stderr, "error data line(index=%d, char=%c, value=%d, length=%d): %s\n", i+1, c, c, len, line);
                exit(0);
            }
            s_builder_add_char(sb, c);
        }
        if(n_locs==0) n_locs=sb->size;
        else if(n_locs!=sb->size){
            fprintf(stderr, "error data: the %dth line's length is not equal before!\n", (list->size+1));
            exit(0);
        }
        n_sample++;
        alist_l_add(list, (int64_t)str_copy_with_len(sb->str, sb->size));
    }
    free_s_builder(sb);
    gz_stream_destory(gz1);

    data=my_new(n_locs*n_sample, sizeof(char));
    for(i=0;i<n_sample;i++){
        char *str=(char *)list->elementData[i];
        for(j=0;j<n_locs;j++) set_data(j, i, str[j]);
        free(str);
    }
    free_alist_l(list);

    read_map();
}

void read_vcf(){
    int i, j, len, num;

    n_sample=0;
    n_locs=0;

    SBuilder *sb=new_s_builder(1000000);

    GzStream *gz1=gz_stream_open(dataFile, "r");
    char *chr=NULL;
    while(gz_read_util(gz1, '\n', line, max_line_length, &len)){
        len=chmop_with_len(line, len);
        //--
        if(line[0]=='#') continue;
        num=str_tab_index(line, '\t', tabs);
        if(num<10){
            fprintf(stderr, "error data line: %s\n", line);
            exit(0);
        }
        if(chr==NULL) chr=str_copy_with_len(line, tabs[0]);
        else if(strncmp(line, chr, tabs[0])!=0){
            fprintf(stderr, "one vcf should contains only one chr, error line: %s\n", line);
            exit(0);
        }
        //--
        for(j=9;j<num;j++){
            i=tabs[j-1];
            char c1=line[i+1];
            char c2=line[i+3];
            s_builder_add_char(sb, c1);
            s_builder_add_char(sb, c2);
            if(c1!='0' && c1!='1' && c2!='0' && c2!='1'){
                fprintf(stderr, "error data line: %s\n", line);
                exit(0);
            }
        }
        n_locs++;
    }
    if(chr) free(chr);
    gz_stream_destory(gz1);

    n_sample=sb->size/n_locs;
    data=sb->str;
    free(sb);

    read_map();
}

void read_map(){
    int i, j, len, num;

    map_distances=my_new(n_locs, sizeof(Map));

    GzStream *gz1=gz_stream_open(mapFile, "r");
    int index=0;
    while(gz_read_util(gz1, '\n', line, max_line_length, &len)){
        len=chmop_with_len(line, len);
        //--
        if(line[0]=='#') continue;
        for(i=0;i<len;i++){
            if(line[i]==' ') line[i]='\t';
        }
        num=str_tab_index(line, '\t', tabs);
        if(num<4){
            fprintf(stderr, "error map line: %s\n", line);
            exit(0);
        }
        if(index>=n_locs){
            fprintf(stderr, "error map file: map file's row number is larger than data's locus!\n");
            exit(0);
        }
        //-- chr
        i=-1;
        j=tabs[0];
        line[j]='\0';
        map_distances[index].chr=str_copy_with_len(line+i+1, (j-i+1));
        //-- rs
        i=tabs[0];
        j=tabs[1];
        line[j]='\0';
        map_distances[index].rs=str_copy_with_len(line+i+1, (j-i+1));
        //-- genetic_distance
        i=tabs[1];
        j=tabs[2];
        line[j]='\0';
        map_distances[index].genetic_distance=atof(line+i+1);
        //-- physical_distance
        i=tabs[2];
        j=tabs[3];
        line[j]='\0';
        map_distances[index].physical_distance=atoi(line+i+1);
        //--
        index++;
    }
    gz_stream_destory(gz1);
}

void read_tped(){
    int i, j, len, num;

    n_locs=0;
    n_sample=0;

    {
        GzStream *gz1=gz_stream_open(dataFile, "r");
        while(gz_read_util(gz1, '\n', line, max_line_length, &len)){
            len=chmop_with_len(line, len);
            //--
            if(line[0]=='#') continue;
            n_locs++;
        }
        gz_stream_destory(gz1);
    }

    map_distances=my_new(n_locs, sizeof(Map));

    SBuilder *sb=new_s_builder(1000000);

    GzStream *gz1=gz_stream_open(dataFile, "r");
    char *chr=NULL;
    int index=0;
    while(gz_read_util(gz1, '\n', line, max_line_length, &len)){
        len=chmop_with_len(line, len);
        //--
        if(line[0]=='#') continue;
        for(i=0;i<len;i++){
            if(line[i]==' ') line[i]='\t';
        }
        num=str_tab_index(line, '\t', tabs);
        if(num<6){
            fprintf(stderr, "error data line: %s\n", line);
            exit(0);
        }
        if(chr==NULL) chr=str_copy_with_len(line, tabs[0]);
        else if(strncmp(line, chr, tabs[0])!=0){
            fprintf(stderr, "one .tped file should contains only one chr, error line: %s\n", line);
            exit(0);
        }
        //-- chr
        i=-1;
        j=tabs[0];
        line[j]='\0';
        map_distances[index].chr=str_copy_with_len(line+i+1, (j-i+1));
        //-- rs
        i=tabs[0];
        j=tabs[1];
        line[j]='\0';
        map_distances[index].rs=str_copy_with_len(line+i+1, (j-i+1));
        //-- genetic_distance
        i=tabs[1];
        j=tabs[2];
        line[j]='\0';
        map_distances[index].genetic_distance=atof(line+i+1);
        //-- physical_distance
        i=tabs[2];
        j=tabs[3];
        line[j]='\0';
        map_distances[index].physical_distance=atoi(line+i+1);
        //--
        index++;
        //--
        for(i=tabs[3]+1;i<len;i++){
            char c=line[i];
            if(c!='\t'){
                if(c!='0' && c!='1'){
                    fprintf(stderr, "error data line: %s\n", line);
                    exit(0);
                }
                s_builder_add_char(sb, c);
            }
        }
    }
    if(chr) free(chr);
    gz_stream_destory(gz1);

    n_sample=sb->size/n_locs;
    data=sb->str;
    free(sb);
}

inline void set_data(int i_locus, int i_sample, char c){
    data[i_locus*n_sample+i_sample]=c;
}

inline char get_data(int i_locus, int i_sample){
    return data[i_locus*n_sample+i_sample];
}

//====================================

void run_one1(int i){
    int j, k;
    int start=window_radius+starts_thread[i];
    int end=window_radius+ends_thread[i];

    AList_i *locus=new_alist_i(16);
    all_processed_locus[i]=(int64_t)locus;

    for(i=start;i<end;i++){
        if(region_start!=-1 && region_end!=-1){
            int dis=map_distances[i].physical_distance;
            if(dis<region_start || dis>region_end) continue;
        }
        //--
        double derived_allele_frequency=0;
        AList_i *list0=new_alist_i(n_sample);
        AList_i *list1=new_alist_i(n_sample);
        for(j=0;j<n_sample;j++){
            if(get_data(i, j)=='0'){
                alist_i_add(list0, j);
            }else{
                derived_allele_frequency++;
                alist_i_add(list1, j);
            }
        }
        derived_allele_frequency/=(double)n_sample;
        if(derived_allele_frequency<min_allele_frequency || derived_allele_frequency>(1.0-min_allele_frequency)) goto next;
        if(list0->size<2 || list1->size<2) goto next;
        results[i].derived_allele_frequency=derived_allele_frequency;
        //--
        alist_i_add(locus, i);
        //--
        next:
        free_alist_l(list0);
        free_alist_l(list1);
    }
}

void run_one2(int i){
    int j, k;

    {
        AList_i *list0=new_alist_i(n_sample);
        AList_i *list1=new_alist_i(n_sample);
        for(j=0;j<n_sample;j++){
            if(get_data(i, j)=='0') alist_i_add(list0, j);
            else alist_i_add(list1, j);
        }
        //--
        double iHH0;
        AList_i *poss0=new_alist_i(16);
        AList_d *ehhs0=new_alist_d(16);
        if(calculate_EHH(i, list0, &iHH0, poss0, ehhs0)){
            results[i].iHH0=iHH0;
            if(outFile2){
                results[i].poss0=poss0;
                results[i].ehhs0=ehhs0;
            }else{
                free_alist_i(poss0);
                free_alist_d(ehhs0);
            }
        }else{
            free_alist_i(poss0);
            free_alist_d(ehhs0);
            memset(results+i, 0, sizeof(Result));
            goto next;
        }
        //--
        double iHH1;
        AList_i *poss1=new_alist_i(16);
        AList_d *ehhs1=new_alist_d(16);
        if(calculate_EHH(i, list1, &iHH1, poss1, ehhs1)){
            results[i].iHH1=iHH1;
            if(outFile2){
                results[i].poss1=poss1;
                results[i].ehhs1=ehhs1;
            }else{
                free_alist_i(poss1);
                free_alist_d(ehhs1);
            }
        }else{
            free_alist_i(poss1);
            free_alist_d(ehhs1);
            memset(results+i, 0, sizeof(Result));
            goto next;
        }
        //--
        double group_iHH0;
        double group_iHH1;
        if(group_size<1.0){
            AList_i *group_poss0=new_alist_i(16);
            AList_d *group_ehhs0=new_alist_d(16);
            if(calculate_group_EHH(i, list0, &group_iHH0, group_poss0, group_ehhs0)){
                results[i].group_iHH0=group_iHH0;
                if(outFile2){
                    results[i].group_poss0=group_poss0;
                    results[i].group_ehhs0=group_ehhs0;
                }else{
                    free_alist_i(group_poss0);
                    free_alist_d(group_ehhs0);
                }
            }else{
                free_alist_i(group_poss0);
                free_alist_d(group_ehhs0);
                memset(results+i, 0, sizeof(Result));
                goto next;
            }
            //--
            AList_i *group_poss1=new_alist_i(16);
            AList_d *group_ehhs1=new_alist_d(16);
            if(calculate_group_EHH(i, list1, &group_iHH1, group_poss1, group_ehhs1)){
                results[i].group_iHH1=group_iHH1;
                if(outFile2){
                    results[i].group_poss1=group_poss1;
                    results[i].group_ehhs1=group_ehhs1;
                }else{
                    free_alist_i(group_poss1);
                    free_alist_d(group_ehhs1);
                }
            }else{
                free_alist_i(group_poss1);
                free_alist_d(group_ehhs1);
                memset(results+i, 0, sizeof(Result));
                goto next;
            }
        }
        //--
        results[i].uiHS=log(iHH1/iHH0);
        if(group_size<1.0){
            results[i].uRiHS1=log(group_iHH1/iHH1);
            results[i].uRiHS0=log(group_iHH0/iHH0);
            results[i].uiHSL=log(group_iHH1/group_iHH0);
        }
        //--
        next:
        free_alist_l(list0);
        free_alist_l(list1);
    }
}

int calculate_EHH(int locus, AList_i *list, double *res_iHH, AList_i *poss, AList_d *ehhs){
    int i, j, k, len, flag, left_1, right_1, size=list->size;

    int is_succeed=1;

    double base=(size*(size-1))/2.0;

    AList_d *ehhs_left=new_alist_d(16);
    AList_d *ehhs_right=new_alist_d(16);

    AList_l *seqs=new_alist_l(16);

    //-- left
    int cur_locus=locus;
    char *seq=my_new(size+1, sizeof(char));
    for(i=0;i<size;i++) seq[i]='1';
    alist_l_add(seqs, (int64_t)seq);
    alist_d_add(ehhs_left, 1.0);
    //--
    while(1){
        cur_locus--;
        if(cur_locus==0){is_succeed=0;break;}
        int dis_span1=map_distances[locus].physical_distance-map_distances[cur_locus].physical_distance;
        if(dis_span1>max_extend) break;
        int dis_span2=map_distances[cur_locus+1].physical_distance-map_distances[cur_locus].physical_distance;
        if(dis_span2>max_gap){is_succeed=0;break;}
        //--
        double ehh=0.0;
        char *s1=get_seq(cur_locus, list, &flag);
        if(flag) ehh=ehhs_left->elementData[ehhs_left->size-1];
        else{
            AList_l *tmp=new_alist_l(16);
            for(i=0;i<seqs->size;i++){
                char *s2=(char *)seqs->elementData[i];
                char *left=my_new(size+1, sizeof(char));
                char *right=my_new(size+1, sizeof(char));
                seq_and_nor_and(s2, s1, size, left, right, &left_1, &right_1);
                if(left_1>1){
                    ehh+=((left_1*(left_1-1))/2.0);
                    alist_l_add(tmp, (int64_t)left);
                }else free(left);
                if(right_1>1){
                    ehh+=((right_1*(right_1-1))/2.0);
                    alist_l_add(tmp, (int64_t)right);
                }else free(right);
                free(s2);
            }
            free_alist_l(seqs);
            seqs=tmp;
            ehh/=base;
        }
        free(s1);
        alist_d_add(ehhs_left, ehh);
        if(ehh<ehh_min_value) break;
    }
    free_string_list(seqs);

    //-- right
    cur_locus=locus;
    seqs=new_alist_l(16);
    seq=my_new(size+1, sizeof(char));
    for(i=0;i<size;i++) seq[i]='1';
    alist_l_add(seqs, (int64_t)seq);
    alist_d_add(ehhs_right, 1.0);
    //--
    while(1){
        cur_locus++;
        if(cur_locus==(n_locs-1)){is_succeed=0;break;}
        //--
        int dis_span1=map_distances[cur_locus].physical_distance-map_distances[locus].physical_distance;
        if(dis_span1>max_extend) break;
        int dis_span2=map_distances[cur_locus].physical_distance-map_distances[cur_locus-1].physical_distance;
        if(dis_span2>max_gap){is_succeed=0;break;}
        //--
        double ehh=0.0;
        char *s1=get_seq(cur_locus, list, &flag);
        if(flag) ehh=ehhs_right->elementData[ehhs_right->size-1];
        else{
            AList_l *tmp=new_alist_l(16);
            for(i=0;i<seqs->size;i++){
                char *s2=(char *)seqs->elementData[i];
                char *left=my_new(size+1, sizeof(char));
                char *right=my_new(size+1, sizeof(char));
                seq_and_nor_and(s2, s1, size, left, right, &left_1, &right_1);
                if(left_1>1){
                    ehh+=((left_1*(left_1-1))/2.0);
                    alist_l_add(tmp, (int64_t)left);
                }else free(left);
                if(right_1>1){
                    ehh+=((right_1*(right_1-1))/2.0);
                    alist_l_add(tmp, (int64_t)right);
                }else free(right);
                free(s2);
            }
            free_alist_l(seqs);
            seqs=tmp;
            ehh/=base;
        }
        free(s1);
        alist_d_add(ehhs_right, ehh);
        if(ehh<ehh_min_value) break;
    }
    free_string_list(seqs);

    double iHH=0.0;
    for(i=1;i<ehhs_left->size;i++){
        cur_locus=locus-i;
        double gd=map_distances[cur_locus+1].genetic_distance-map_distances[cur_locus].genetic_distance;
        int pd=map_distances[cur_locus+1].physical_distance-map_distances[cur_locus].physical_distance;
        if(pd>scale_value) gd*=((double)scale_value/(double)pd);
        double height=(ehhs_left->elementData[i-1]+ehhs_left->elementData[i])/2.0;
        iHH+=(gd*height);
    }

    for(i=1;i<ehhs_right->size;i++){
        cur_locus=locus+i;
        double gd=map_distances[cur_locus].genetic_distance-map_distances[cur_locus-1].genetic_distance;
        int pd=map_distances[cur_locus].physical_distance-map_distances[cur_locus-1].physical_distance;
        if(pd>scale_value) gd*=((double)scale_value/(double)pd);
        double height=(ehhs_right->elementData[i-1]+ehhs_right->elementData[i])/2.0;
        iHH+=(gd*height);
    }

    *(res_iHH)=iHH;
    for(i=ehhs_left->size-1;i>=0;i--){
        cur_locus=locus-i;
        alist_i_add(poss, cur_locus);
        alist_d_add(ehhs, ehhs_left->elementData[i]);
    }
    for(i=1;i<ehhs_right->size;i++){
        cur_locus=locus+i;
        alist_i_add(poss, cur_locus);
        alist_d_add(ehhs, ehhs_right->elementData[i]);
    }

    if(poss->size==0 || ehhs->size==0){
        fprintf(stderr, "error1: %d\t%d\n", ehhs_left->size, ehhs_right->size);
        exit(0);
    }

    free_alist_d(ehhs_left);
    free_alist_d(ehhs_right);
    return is_succeed;
}

char *get_seq(int l, AList_i *list, int *flag){
    SBuilder *sb=new_s_builder(list->size);
    int i, num=0;
    for(i=0;i<list->size;i++){
        char c=get_data(l, list->elementData[i]);
        s_builder_add_char(sb, c);
        if(c=='1') num++;
    }
    char *res=sb->str;
    free(sb);
    *(flag)=(num==0 || num==list->size) ? 1:0;
    return res;
}

void seq_and_nor_and(char *s1, char *s2, int size, char *left, char *right, int *res_left_1, int *res_right_1){
    int i, left_1=0, right_1=0;

    for(i=0;i<size;i++){
        char c1=s1[i];
        char c2=s2[i];
        if(c1=='0' && c2=='0'){
            left[i]='0';
            right[i]='0';
        }else if(c1=='0' && c2=='1'){
            left[i]='0';
            right[i]='0';
        }else if(c1=='1' && c2=='0'){
            left[i]='0';
            right[i]='1';
            right_1++;
        }else{
            left[i]='1';
            right[i]='0';
            left_1++;
        }
    }
    *(res_left_1)=left_1;
    *(res_right_1)=right_1;
}

void free_string_list(AList_l *list){
    int i;
    for(i=0;i<list->size;i++){
        char *str=(char *)list->elementData[i];
        free(str);
    }
    free_alist_l(list);
}

//====================================

int calculate_group_EHH(int locus, AList_i *sub_sample_indexes, double *res_iHH, AList_i *poss, AList_d *ehhs){
    int i, j, k, size=sub_sample_indexes->size, l_seq=2*window_radius+1;

    int a=round((double)size*group_size);
    int b=round(0.02*(double)n_sample);
    int R=min(max(max(a, b), 8), size);

    AList_l *seqs=new_alist_l(size);
    AList_i *indexes=new_alist_i(size);
    int index=0;

    int l_table=2*size;
    int64_t *table=my_new(l_table, sizeof(int64_t));
    for(i=0;i<size;i++){
        int i_sample=sub_sample_indexes->elementData[i];
        char *seq=my_new(l_seq+1, sizeof(char));
        for(j=locus-window_radius,k=0;j<=(locus+window_radius);j++,k++){
            seq[k]=get_data(j, i_sample);
        }
        //--
        Entry_dup *e=put_hash_seqs(table, l_table, seq, l_seq, index);
        alist_i_add(indexes, e->index);
        if(e->freq==1){
            alist_l_add(seqs, (int64_t)e);
            index++;
        }
        //--
        free(seq);
    }

    int l_seqs=seqs->size;
    int *sub_distances=my_new(l_seqs*l_seqs, sizeof(int));
    for(i=0;i<l_seqs;i++){
        char *s1=((Entry_dup *)(seqs->elementData[i]))->seq;
        for(j=0;j<l_seqs;j++){
            if(i==j){
                sub_distances[i*l_seqs+j]=0;
                continue;
            }
            int pre=sub_distances[j*l_seqs+i];
            if(pre){
                sub_distances[i*l_seqs+j]=pre;
                continue;
            }
            char *s2=((Entry_dup *)(seqs->elementData[j]))->seq;
            sub_distances[i*l_seqs+j]=hamming_distance(s1, s2, l_seq);
        }
    }

    int *distances=my_new(size*size, sizeof(int));
    for(i=0;i<size;i++){
        int ii=indexes->elementData[i];
        for(j=0;j<size;j++){
            int jj=indexes->elementData[j];
            distances[i*size+j]=sub_distances[ii*l_seqs+jj];
        }
    }
    for(i=0;i<l_table;i++) free_entry_dup((Entry_dup *)table[i]);
    free(sub_distances);
    free_alist_i(indexes);

    memset(table, 0, l_table*sizeof(int64_t));
    seqs->size=0;

    char **tmps=my_new(size, sizeof(char *));
    for(i=0;i<size;i++) tmps[i]=my_new(20, sizeof(char));
    for(i=0;i<size;i++){
        int *dis=distances+(i*size);
        for(j=0;j<size;j++) sprintf(tmps[j], "%d\t%d", j, dis[j]);
        java_sort_void(tmps, 0, size, (int (*)(void *, void *))compare_string);
        char *flag=my_new(size, sizeof(char));
        for(j=0;j<R;j++){
            char *str=tmps[j];
            k=0;
            while(str[++k]!='\t');
            str[k]='\0';
            flag[atoi(str)]=1;
        }
        //--
        char *tmp=my_new(10*R, sizeof(char));
        char *copy=tmp;
        for(j=0;j<size;j++){
            if(flag[j]) tmp+=sprintf(tmp, "%d\t", j);
        }
        int l_copy=tmp-copy-1;
        copy[l_copy]='\0';
        //--
        Entry_dup *e=put_hash_seqs(table, l_table, copy, l_copy, 0);
        if(e->freq==1) alist_l_add(seqs, (int64_t)e);
        //--
        free(copy);
        free(flag);
    }
    freeArray(size, tmps);
    free(distances);

    AList_l *lists=new_alist_l(seqs->size);
    AList_i *lists_freq=new_alist_i(seqs->size);

    int *tmp_tabs=my_new(2*R, sizeof(int));
    for(i=0;i<seqs->size;i++){
        Entry_dup *e=(Entry_dup *)seqs->elementData[i];
        AList_i *sub_list=new_alist_i(R);
        AList_i *sub_indexes=string_2_alist_i(e->seq, tmp_tabs);
        for(j=0;j<sub_indexes->size;j++){
            int sub_index=sub_indexes->elementData[j];
            alist_i_add(sub_list, sub_sample_indexes->elementData[sub_index]);
        }
        free_alist_i(sub_indexes);
        alist_l_add(lists, (int64_t)sub_list);
        alist_i_add(lists_freq, e->freq);
    }
    for(i=0;i<l_table;i++) free_entry_dup((Entry_dup *)table[i]);
    free(table);
    free(tmp_tabs);
    free_alist_l(seqs);

    int unsucceed_num=0;
    double total_iHH=0.0;
    int min_locus=1000000000, max_locus=-1;
    AList_l *lists_poss=new_alist_l(lists->size);
    AList_l *lists_ehhs=new_alist_l(lists->size);
    for(i=0;i<lists->size;i++){
        AList_i *list=(AList_i *)lists->elementData[i];
        int freq=lists_freq->elementData[i];
        //--
        double iHH;
        AList_i *tmp_poss=new_alist_i(16);
        AList_d *tmp_ehhs=new_alist_d(16);
        if(calculate_EHH(locus, list, &iHH, tmp_poss, tmp_ehhs)==0) unsucceed_num+=freq;
        total_iHH+=(iHH*freq);
        int min=tmp_poss->elementData[0];
        int max=tmp_poss->elementData[tmp_poss->size-1];
        if(min_locus>min) min_locus=min;
        if(max_locus<max) max_locus=max;
        //--
        alist_l_add(lists_poss, (int64_t)tmp_poss);
        alist_l_add(lists_ehhs, (int64_t)tmp_ehhs);
        free_alist_i(list);
    }
    free_alist_l(lists);

    int l_array=(max_locus-min_locus+1);
    if(l_array>0){
        double *array=my_new(l_array, sizeof(double));
        for(i=0;i<lists_freq->size;i++){
            int freq=lists_freq->elementData[i];
            AList_i *tmp_poss=(AList_i *)lists_poss->elementData[i];
            AList_d *tmp_ehhs=(AList_d *)lists_ehhs->elementData[i];
            for(j=0;j<tmp_poss->size;j++){
                int cur_locus=tmp_poss->elementData[j];
                double cur_ehh=tmp_ehhs->elementData[j];
                array[cur_locus-min_locus]+=(cur_ehh*freq);
            }
        }
        *(res_iHH)=total_iHH/(double)size;
        for(i=0;i<l_array;i++){
            alist_i_add(poss, min_locus+i);
            alist_d_add(ehhs, array[i]/(double)size);
        }
        free(array);
    }
    for(i=0;i<lists_poss->size;i++){
        free_alist_i((AList_i *)lists_poss->elementData[i]);
        free_alist_d((AList_d *)lists_ehhs->elementData[i]);
    }
    free_alist_l(lists_poss);
    free_alist_l(lists_ehhs);
    free_alist_i(lists_freq);

    double rate=(double)unsucceed_num/(double)size;
    return (is_trunc_ok || (rate<trunc_rate)) ? 1:0;
}

int32_t hash_mem(char *key, int l_key){
    int32_t i, h=0;
    for(i=0;i<l_key;i++) h=31*h+key[i];
    return h;
}

Entry_dup *put_hash_seqs(int64_t *table, int l_table, char *seq, int l_seq, int index){
    int hash=hash_mem(seq, l_seq);
    int h_index=((unsigned int)hash)%l_table;
    if(h_index<0){
        fprintf(stderr, "error hash!\n");
        exit(0);
    }

    Entry_dup *e=(Entry_dup *)table[h_index];
    while(e){
        if(e->hash==hash && e->l_seq==l_seq && memcmp(e->seq, seq, l_seq)==0){
            e->freq++;
            return e;
        }
        e=(Entry_dup *)e->next;
    }
    e=my_new(1, sizeof(Entry_dup));
    e->hash=hash;
    e->seq=str_copy_with_len(seq, l_seq);
    e->l_seq=l_seq;
    e->index=index;
    e->freq=1;
    e->next=table[h_index];
    table[h_index]=(int64_t)e;
    return e;
}

inline void free_entry_dup(Entry_dup *e){
    if(!e) return;
    free_entry_dup((Entry_dup *)(e->next));
    free(e->seq);
    free(e);
}

int compare_string(char *s1, char *s2){
    int i=0;
    while(s1[++i]!='\t');
    int v1=atoi(s1+i+1);
    //--
    i=0;
    while(s2[++i]!='\t');
    int v2=atoi(s2+i+1);
    //--
    if(v1>v2) return 1;
    if(v1<v2) return -1;
    return 0;
}

int hamming_distance(char *s1, char *s2, int size){
    int i, j, num=0;
    for(i=0;i<size;i++){
        if(s1[i]!=s2[i]) num++;
    }
    return num;
}

AList_i *string_2_alist_i(char *str, int *tabs){
    int i, num=str_tab_index(str, '\t', tabs);
    AList_i *list=new_alist_i(num);
    for(i=0;i<num;i++) str[tabs[i]]='\0';
    alist_i_add(list, atoi(str));
    for(i=1;i<num;i++) alist_i_add(list, atoi(str+tabs[i-1]+1));
    return list;
}

//====================================

void write_output1(){
    int i, j, len;

    GzStream *out=gz_stream_open(outFile1, "w");

    if(group_size<1.0) len=sprintf(line, "#locusID\tchr\tphysicalPos\t'1'_freq\tiHH1\tiHH0\tiHHL1\tiHHL0\tuiHS\tuRiHS1\tuRiHS0\tuiHSL\n");
    else len=sprintf(line, "#locusID\tchr\tphysicalPos\t'1'_freq\tiHH1\tiHH0\tuiHS\n");

    gz_write(out, line, len);
    for(i=0;i<n_locs;i++){
        Result res=results[i];
        Map map=map_distances[i];
        if(res.derived_allele_frequency<min_allele_frequency || res.derived_allele_frequency>(1.0-min_allele_frequency)) continue;
        if(group_size<1.0){
            int len=sprintf(line, "%s\t%s\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n", map.rs, map.chr, map.physical_distance, res.derived_allele_frequency, res.iHH1, res.iHH0, res.group_iHH1, res.group_iHH0, res.uiHS, res.uRiHS1, res.uRiHS0, res.uiHSL);
            gz_write(out, line, len);
        }else{
            int len=sprintf(line, "%s\t%s\t%d\t%f\t%f\t%f\t%f\n", map.rs, map.chr, map.physical_distance, res.derived_allele_frequency, res.iHH1, res.iHH0, res.uiHS);
            gz_write(out, line, len);
        }
    }
    gz_stream_destory(out);
}

void write_output2(){
    int i, j, len;

    GzStream *out=gz_stream_open(outFile2, "w");

    if(group_size<1.0) len=sprintf(line, "#locusID\tchr\tphysicalPos\t'1'_freq\tiHH1\tiHH0\tiHHL1\tiHHL0\tuiHS\tuRiHS1\tuRiHS0\tuiHSL\tEHH1_hard_pos\tEHH1_hard\tEHH0_hard_pos\tEHH0_hard\tEHH1_soft_pos\tEHH1_soft\tEHH0_soft_pos\tEHH0_soft\n");
    else len=sprintf(line, "#locusID\tchr\tphysicalPos\t'1'_freq\tiHH1\tiHH0\tuiHS\tEHH1_hard_pos\tEHH1_hard\tEHH0_hard_pos\tEHH0_hard\n");
    gz_write(out, line, len);

    for(i=0;i<n_locs;i++){
        Result res=results[i];
        Map map=map_distances[i];
        if(res.derived_allele_frequency<min_allele_frequency || res.derived_allele_frequency>(1.0-min_allele_frequency)) continue;
        if(group_size<1.0){
            len=sprintf(line, "%s\t%s\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", map.rs, map.chr, map.physical_distance, res.derived_allele_frequency, res.iHH1, res.iHH0, res.group_iHH1, res.group_iHH0, res.uiHS, res.uRiHS1, res.uRiHS0, res.uiHSL);
            gz_write(out, line, len);
            //--
            gz_write_char(out, '\t');len=list_poss_2_string(res.poss1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.ehhs1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_poss_2_string(res.poss0);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.ehhs0);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_poss_2_string(res.group_poss1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.group_ehhs1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_poss_2_string(res.group_poss0);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.group_ehhs0);gz_write(out, line, len);
            gz_write_char(out, '\n');
        }else{
            len=sprintf(line, "%s\t%s\t%d\t%f\t%f\t%f\t%f", map.rs, map.chr, map.physical_distance, res.derived_allele_frequency, res.iHH1, res.iHH0, res.uiHS);
            gz_write(out, line, len);
            //--
            gz_write_char(out, '\t');len=list_poss_2_string(res.poss1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.ehhs1);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_poss_2_string(res.poss0);gz_write(out, line, len);
            gz_write_char(out, '\t');len=list_ehhs_2_string(res.ehhs0);gz_write(out, line, len);
            gz_write_char(out, '\n');
        }
    }
    gz_stream_destory(out);
}

int list_poss_2_string(AList_i *list){
    char *str=line;

    str+=sprintf(str, "[");
    if(list){
        int i;
        for(i=0;i<list->size;i++){
            int locus=list->elementData[i];
            str+=sprintf(str, "%d, ", map_distances[locus].physical_distance);
        }
        str-=2;
    }
    str+=sprintf(str, "]");

    int len=str-line;
    line[len]='\0';
    return len;
}

int list_ehhs_2_string(AList_d *list){
    char *str=line;

    str+=sprintf(str, "[");
    if(list){
        int i;
        for(i=0;i<list->size;i++){
            str+=sprintf(str, "%f, ", list->elementData[i]);
        }
        str-=2;
    }
    str+=sprintf(str, "]");

    int len=str-line;
    line[len]='\0';
    return len;
}

//====================================

AList_l *get_all_calc_results_file(){
    int i, j;

    AList_l *list1=get_file_list(norm_input_dir);
    AList_l *list2=new_alist_l(list1->size);
    for(i=0;i<list1->size;i++){
        char *file=(char *)list1->elementData[i];
        char *chr=get_file_chr(file);
        int len=sprintf(loginfo, "%s\t%s", file, chr);
        alist_l_add(list2, (int64_t)str_copy_with_len(loginfo, len));
        free(file);
        free(chr);
    }
    free_alist_l(list1);

    if(list2->size>1) java_sort_void(list2->elementData, 0, list2->size, (int (*)(void *, void *))compare_chr);
    for(i=0;i<list2->size;i++){
        char *str=(char *)list2->elementData[i];
        j=0;
        while(str[++j]!='\t');
        str[j]='\0';
    }

    sprintf(loginfo, "chr_file_num=%d", list2->size);mylog(loginfo);

    return list2;
}

char *get_file_chr(char *file){
    int i, index, num, len;

    GzStream *gz1=gz_stream_open(file, "r");

    gz_read_util(gz1, '\n', line, maxline, &len);
    num=str_tab_index(line, '\t', tabs);
    for(i=0;i<num;i++) line[tabs[i]]='\0';

    index=-1;
    if(strcmp(line+1, "chr")==0) index=0;
    else{
        for(i=1;i<num;i++){
            if(strcmp(line+tabs[i-1]+1, "chr")==0) index=i;
        }
    }
    if(index==-1){
        fprintf(stderr, "error: get file chr!\n");
        exit(0);
    }

    gz_read_util(gz1, '\n', line, maxline, &len);
    num=str_tab_index(line, '\t', tabs);
    for(i=0;i<num;i++) line[tabs[i]]='\0';

    gz_stream_destory(gz1);

    if(index==0) return str_copy(line);
    else return str_copy(line+tabs[index-1]+1);
}

int compare_chr(char *str1, char *str2){
    int i=0;
    while(str1[++i]!='\t');
    char *chr1=str_copy(str1+i+1);
    str_2_lower_no_copy(chr1);

    i=0;
    while(str2[++i]!='\t');
    char *chr2=str_copy(str2+i+1);
    str_2_lower_no_copy(chr2);

    char *copy_chr1=chr1;
    char *copy_chr2=chr2;

    int index1=str_indexOf(chr1, "chr");
    if(index1!=-1) chr1+=(index1+3);

    int index2=str_indexOf(chr2, "chr");
    if(index2!=-1) chr2+=(index2+3);

    int is_int1=is_integer(chr1);
    int is_int2=is_integer(chr2);

    if(is_int1 && is_int2){
        int a=atoi(chr1);
        int b=atoi(chr2);
        if(a>b) goto next1;
        if(a<b) goto next2;
        goto next0;
    }

    if(is_int1 && !is_int2) goto next2;

    if(!is_int1 && is_int2) goto next1;

    if(!is_int1 && !is_int2){
        char c1=chr1[0];
        char c2=chr2[0];
        if(c1=='x') goto next2;
        if(c2=='x') goto next1;
        if(c1=='y') goto next2;
        if(c2=='y') goto next1;
        if(c1=='m') goto next2;
        if(c2=='m') goto next1;
        int index=strcmp(chr1, chr2);
        if(index==0) goto next0;
        if(index<0) goto next2;
        if(index>0) goto next1;
    }

    next0:
    free(copy_chr1);
    free(copy_chr2);
    return 0;

    next1:
    free(copy_chr1);
    free(copy_chr2);
    return 1;

    next2:
    free(copy_chr1);
    free(copy_chr2);
    return -1;
}

int get_file_header_index(char *file, char *element){
    int i, num, len;

    GzStream *gz1=gz_stream_open(file, "r");

    gz_read_util(gz1, '\n', line, maxline, &len);
    num=str_tab_index(line, '\t', tabs);
    for(i=0;i<num;i++) line[tabs[i]]='\0';

    gz_stream_destory(gz1);

    if(strcmp(line+1, element)==0) return 0;
    else{
        for(i=1;i<num;i++){
            char *str1= str_2_lower_with_copy(line+tabs[i-1]+1);
            char *str2= str_2_lower_with_copy(element);
            if(strcmp(str1, str2)==0){
                free(str1);
                free(str2);
                return i;
            }
            free(str1);
            free(str2);
        }
    }

    return -1;
}

void read_norm_data(AList_l *files){
    int i, j, num, len, index=0, span=1000000;

    l_norm_data=span;
    norm_data=my_new(l_norm_data, sizeof(NormData));

    int f_index=0;

    for(i=0;i<files->size;i++){
        char *file=(char *)files->elementData[i];
        sprintf(loginfo, "reading %s...", file);mylog(loginfo);
        //--
        int index1=get_file_header_index(file, "uiHS");
        int index2=get_file_header_index(file, "uRiHS1");
        int index3=get_file_header_index(file, "uRiHS0");
        int index4=get_file_header_index(file, "uiHSL");
        int index5=get_file_header_index(file, "iHH1");
        int index6=get_file_header_index(file, "iHH0");
        int index7=get_file_header_index(file, "iHHL1");
        int index8=get_file_header_index(file, "iHHL0");
        //--
        GzStream *gz1=gz_stream_open(file, "r");
        while(gz_read_util(gz1, '\n', line, maxline, &len)){
            len=chmop_with_len(line, len);
            if(line[0]=='#') continue;
            //--
            if(index>=l_norm_data){
                norm_data=my_renew(norm_data, (l_norm_data+span)*sizeof(NormData));
                memset(norm_data+l_norm_data, 0, span*sizeof(NormData));
                l_norm_data+=span;
            }
            //--
            num=str_tab_index(line, '\t', tabs);
            for(j=0;j<num;j++) line[tabs[j]]='\0';
            //--
            if(atof(line+tabs[index5-1]+1)==0.0 || atof(line+tabs[index6-1]+1)==0.0 || atof(line+tabs[index7-1]+1)==0.0 || atof(line+tabs[index8-1]+1)==0.0) continue;
            //--
            norm_data[index].rs=str_copy_with_len(line, tabs[0]);
            norm_data[index].chr=str_copy_with_len(line+tabs[0]+1, tabs[1]-tabs[0]-1);
            norm_data[index].physical_distance=atoi(line+tabs[1]+1);
            norm_data[index].freq_1=atof(line+tabs[2]+1);
            norm_data[index].uiHS=atof(line+tabs[index1-1]+1);
            norm_data[index].uRiHS1=atof(line+tabs[index2-1]+1);
            norm_data[index].uRiHS0=atof(line+tabs[index3-1]+1);
            norm_data[index].uiHSL=atof(line+tabs[index4-1]+1);
            //-
            char *freq=line+tabs[2]+1;
            int l_freq=tabs[3]-tabs[2]-1;
            Entry_si *entry= hash_si_get(norm_freq_index, freq);
            if(entry) norm_data[index].num_1=entry->value;
            else{
                norm_data[index].num_1=f_index;
                hash_si_put(norm_freq_index, freq, f_index);
                alist_d_add(norm_all_freqs, atof(freq));
                f_index++;
            }
            //--
            index++;
        }
        gz_stream_destory(gz1);
        //--
        free(file);
    }

    l_norm_data=index;
    n_sample=f_index;
    sprintf(loginfo, "all_locus_number=%d\tfreq_num=%d", l_norm_data, n_sample);mylog(loginfo);
}

void calculate_all_average_variance_one(int index){
    int i, j;

    AList_d *uiHS=new_alist_d(16);
    AList_d *uRiHS1=new_alist_d(16);
    AList_d *uRiHS0=new_alist_d(16);
    AList_d *uiHSL=new_alist_d(16);

    double current=norm_all_freqs->elementData[index];

    AList_i *pos= new_alist_i(16);

    for(i=0;i<n_sample;i++){
        AList_i *list=(AList_i *)norm_talbe[i];
        double d=norm_all_freqs->elementData[i];
        if(fabs(d-current)>(norm_bin_size/2.0)) continue;
        for(j=0;j<list->size;j++){
            NormData nd1=norm_data[list->elementData[j]];
            alist_i_add(pos, nd1.physical_distance);
            if(!isnan(nd1.uiHS) && !isinf(nd1.uiHS)) alist_d_add(uiHS, nd1.uiHS);
            if(!isnan(nd1.uRiHS1) && !isinf(nd1.uRiHS1)) alist_d_add(uRiHS1, nd1.uRiHS1);
            if(!isnan(nd1.uRiHS0) && !isinf(nd1.uRiHS0)) alist_d_add(uRiHS0, nd1.uRiHS0);
            if(!isnan(nd1.uiHSL) && !isinf(nd1.uiHSL)) alist_d_add(uiHSL, nd1.uiHSL);
        }
    }

    calculate_average_variance(uiHS, norm_uiHS_avarage+index, norm_uiHS_variance+index);
    calculate_average_variance(uRiHS1, norm_uRiHS1_avarage+index, norm_uRiHS1_variance+index);
    calculate_average_variance(uRiHS0, norm_uRiHS0_avarage+index, norm_uRiHS0_variance+index);
    calculate_average_variance(uiHSL, norm_uiHSL_avarage+index, norm_uiHSL_variance+index);

    free_alist_i(pos);

    free_alist_d(uiHS);
    free_alist_d(uRiHS1);
    free_alist_d(uRiHS0);
    free_alist_d(uiHSL);
}

void calculate_average_variance(AList_d *list, double *res_average, double *res_variance){
    if(list->size==1){
        *(res_average)=list->elementData[0];
        *(res_variance)=1.0;
        return;
    }

    int i;

    double average=0.0;
    for(i=0;i<list->size;i++) average+=list->elementData[i];
    average/=(double)list->size;

    double variance=0.0;
    for(i=0;i<list->size;i++) variance+=pow(list->elementData[i]-average, 2.0);
    variance/=(double)(list->size-1);
    variance=sqrt(variance);

    *(res_average)=average;
    *(res_variance)=variance;
}

void norm_calculate_one(int thread_index){
    int i, j, start=starts_thread[thread_index], end=ends_thread[thread_index];

    for(i=start;i<end;i++){
        int num=norm_data[i].num_1;
        norm_data[i].iHS=(norm_data[i].uiHS-norm_uiHS_avarage[num])/norm_uiHS_variance[num];
        norm_data[i].RiHS1=(norm_data[i].uRiHS1-norm_uRiHS1_avarage[num])/norm_uRiHS1_variance[num];
        norm_data[i].RiHS0=(norm_data[i].uRiHS0-norm_uRiHS0_avarage[num])/norm_uRiHS0_variance[num];
        norm_data[i].iHSL=(norm_data[i].uiHSL-norm_uiHSL_avarage[num])/norm_uiHSL_variance[num];
        norm_data[i].RiHSL=norm_data[i].iHSL<0.0 ? (pow(norm_data[i].RiHS0, 2.0)+pow(norm_data[i].iHSL, 2.0)):(pow(norm_data[i].RiHS1, 2.0)+pow(norm_data[i].iHSL, 2.0));
        norm_data[i].p_iHSL=norm_cdf(-fabs(norm_data[i].iHSL), 0.0, 1.0)*2.0;
        norm_data[i].p_RiHSL= chi2_cdf_upper(norm_data[i].RiHSL, 2);    //gsl_cdf_chisq_Q(norm_data[i].RiHSL, 2);
    }
}

//====================================







