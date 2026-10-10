#include <stdio.h>
#include <stdlib.h>
#include "files.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_err(STATUS_ERR_NULL);
        return EXIT_FAILURE;
    }

    char out = '\0';
    int has_n = 0;
    Status st = parse_flag(argv[1], &out, &has_n);
    if (st != STATUS_OK) {
        print_err(st);
        return EXIT_FAILURE;
    }

    if ((has_n && argc != 4) || (!has_n && argc != 3)) {
        print_err(STATUS_ERR_NULL);
        return EXIT_FAILURE;
    }

    char *out_name = has_n ? my_strdup(argv[3]) : make_file_name(argv[2]);
    if (!out_name) {
        print_err(STATUS_ERR_MEM);
        return EXIT_FAILURE;
    }

    FILE *fin = fopen(argv[2], "r");
    FILE *fout = fopen(out_name, "w");

    if (!fin) {
        st = STATUS_ERR_OPEN_IN;
    } else if (!fout) {
        st = STATUS_ERR_OPEN_OUT;
    } else {
        switch (out) {
            case 'd':
                st = proc_d(fin, fout);
                break;
            case 'i':
                st = proc_i(fin, fout);
                break;
            case 's':
                st = proc_s(fin, fout);
                break;
            case 'a':
                st = proc_a(fin, fout);
                break;
            default:
                st = STATUS_ERR_UNKNOWN_FLAG;
                break;
        }
    }

    if (fin) fclose(fin);
    if (fout) fclose(fout);
    free(out_name);

    if (st != STATUS_OK) {
        print_err(st);
    }

    return st == STATUS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}