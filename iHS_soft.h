#ifndef IHS_SOFT_H_INCLUDED
#define IHS_SOFT_H_INCLUDED

#include "myfunction.h"
#include "myutil.h"

#define max_line_length 1000000000

typedef struct map_t{
    char *chr;
    char *rs;
    double genetic_distance;
    int physical_distance;
}Map;

typedef struct entry_dup_t{
    int hash;
    char *seq;
    int l_seq;
    int index;
    int freq;
    int64_t next;
}Entry_dup;

typedef struct result_t{
    double derived_allele_frequency;
    double iHH0;
    AList_i *poss0;
    AList_d *ehhs0;
    double iHH1;
    AList_i *poss1;
    AList_d *ehhs1;
    double group_iHH0;
    AList_i *group_poss0;
    AList_d *group_ehhs0;
    double group_iHH1;
    AList_i *group_poss1;
    AList_d *group_ehhs1;
    double uiHS;
    double uRiHS1;
    double uRiHS0;
    double uiHSL;
}Result;

typedef struct norm_data_t{
    char *rs;
    char *chr;
    int physical_distance;
    double freq_1;
    int num_1;
    //--
    double uiHS;
    double uRiHS1;
    double uRiHS0;
    double uiHSL;
    //--
    double iHS;
    double RiHS1;
    double RiHS0;
    double iHSL;
    double RiHSL;
    double p_iHSL;
    double p_RiHSL;
}NormData;

//-- input parameters
char *dataFile;
char *mapFile;
int region_start;
int region_end;
int is_trunc_ok;
double trunc_rate;
double min_allele_frequency;
double ehh_min_value;
int scale_value;
int max_gap;
int window_radius;
double group_size;
int max_extend;
int thread_num;
char *outFile1;
char *outFile2;
//--
char *norm_input_dir;
double norm_bin_size;
char *norm_output;
//--
int64_t n_locs;
int64_t n_sample;
char *data;    //n_locus*n_sample
Map *map_distances;
ThreadPool *pool;
Result *results;
//-- norm
NormData *norm_data;
int l_norm_data;


//====================================
void print_usage0();
//====================================
int main_process1(int argc, char **argv);
void load_parameters1(int argc, char **argv);
void print_usage1();
//====================================
int main_process2(int argc, char **argv);
void load_parameters2(int argc, char **argv);
void print_usage2();
//====================================
void read_hap();
void read_vcf();
void read_map();
void read_tped();
void set_data(int i_locus, int i_sample, char c);
char get_data(int i_locus, int i_sample);
//====================================
void run_one1(int i);
void run_one2(int i);
int calculate_EHH(int locus, AList_i *list, double *res_iHH, AList_i *poss, AList_d *ehhs);
char *get_seq(int l, AList_i *list, int *flag);
void seq_and_nor_and(char *s1, char *s2, int size, char *left, char *right, int *res_left_1, int *res_right_1);
void free_string_list(AList_l *list);
//====================================
int calculate_group_EHH(int locus, AList_i *sub_sample_indexes, double *res_iHH, AList_i *poss, AList_d *ehhs);
int32_t hash_mem(char *key, int l_key);
Entry_dup *put_hash_seqs(int64_t *table, int l_table, char *seq, int l_seq, int index);
void free_entry_dup(Entry_dup *e);
int compare_string(char *s1, char *s2);
int hamming_distance(char *s1, char *s2, int size);
AList_i *string_2_alist_i(char *str, int *tabs);
//====================================
void write_output1();
void write_output2();
int list_poss_2_string(AList_i *list);
int list_ehhs_2_string(AList_d *list);
//====================================
AList_l *get_all_calc_results_file();
char *get_file_chr(char *file);
int compare_chr(char *chr1, char *chr2);
int get_file_header_index(char *file, char *element);
void read_norm_data(AList_l *files);
void calculate_all_average_variance_one(int index);
void calculate_average_variance(AList_d *list, double *res_average, double *res_variance);
void norm_calculate_one(int thread_index);
//====================================

#endif // IHS_SOFT_H_INCLUDED
