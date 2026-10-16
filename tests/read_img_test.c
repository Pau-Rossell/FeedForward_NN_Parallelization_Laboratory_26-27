#include "../code/XXNN-Multinivell-main/common/common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Run from the repository root:
 * cc -O1 -g -fsanitize=address tests/read_img_test.c \
 *   code/XXNN-Multinivell-main/common/common.c -o /tmp/read-img-test
 * /tmp/read-img-test
 */

/* Definitions needed to link the supplied common.c in isolation. */
int batch_size;
float **desired_outputs;
int *Validation;
int num_training_patterns;
int num_test_patterns;
int img_dim_x;
int img_dim_y;
int num_out_layer;
int debug;

static void check_text(const char *text, int rows, int columns,
                       const char *expected) {
    FILE *file = tmpfile();
    assert(file != NULL);
    assert(fputs(text, file) >= 0);
    rewind(file);

    img_dim_x = rows;
    img_dim_y = columns;
    char *image = readImg(file);
    assert(image != NULL);
    assert(memcmp(image, expected, (size_t)rows * columns) == 0);

    free(image);
    fclose(file);
}

static void check_dataset(const char *path, int patterns) {
    FILE *file = fopen(path, "r");
    assert(file != NULL);
    img_dim_x = 32;
    img_dim_y = 32;

    for (int p = 0; p < patterns; p++) {
        char *image = readImg(file);
        assert(image != NULL);
        for (int i = 0; i < 32 * 32; i++)
            assert(image[i] == 0 || image[i] == 1);
        free(image);

        int label;
        assert(fscanf(file, "%d\n", &label) == 1);
        assert(label >= 0 && label <= 9);
    }
    assert(fgetc(file) == EOF);
    fclose(file);
}

int main(void) {
    check_dataset("code/XXNN-Multinivell-main/datasets/optdigits.tra", 1934);
    check_dataset("code/XXNN-Multinivell-main/datasets/optdigits.cv", 946);

    const char square[] = {0, 1, 1, 0};
    const char rectangle[] = {0, 1, 0, 1, 0, 1};
    const char single[] = {1};
    check_text("01\n10\n", 2, 2, square);
    check_text("010\n101\n", 2, 3, rectangle);
    check_text("010\r\n101\r\n", 2, 3, rectangle);
    check_text("1", 1, 1, single);

    puts("Image-reader checks passed: 2,880 OPTDIGITS images and edge cases.");
    return 0;
}
