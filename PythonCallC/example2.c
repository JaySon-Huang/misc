#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <Python/Python.h>
#include <assert.h>

typedef struct backing_store_struct * backing_store_ptr;
typedef struct backing_store_struct {
    void (*read_backing_store) 
        (j_common_ptr cinfo,
         backing_store_ptr info,
         void FAR * buffer_address,
         long file_offset, long byte_count);
    void (*write_backing_store)
        (j_common_ptr cinfo,
         backing_store_ptr info,
         void FAR * buffer_address,
         long file_offset, long byte_count);
    void (*close_backing_store)
        (j_common_ptr cinfo,
         backing_store_ptr info);

    FILE * temp_file;
    char temp_name[64];
} backing_store_info;

struct jvirt_barray_control {
    JBLOCKARRAY mem_buffer;   /* => the in-memory buffer 指向内存缓冲区 */
    JDIMENSION rows_in_array; /* total virtual array height  */
    JDIMENSION blocksperrow;  /* width of array (and of memory buffer) */
    JDIMENSION maxaccess;     /* max rows accessed by access_virt_barray */
    JDIMENSION rows_in_mem;   /* height of memory buffer 在内存中内容的高度 */
    JDIMENSION rowsperchunk;  /* allocation chunk size in mem_buffer */
    JDIMENSION cur_start_row; /* first logical row # in the buffer 内存中内容的第一行的行号 */
    JDIMENSION first_undef_row;   /* row # of first uninitialized row 第一个没有初始化的行的行号，可以看作是含数据的最后一行的后一行 */
    boolean pre_zero;     /* pre-zero mode requested? */
    boolean dirty;        /* do current buffer contents need written? 是否"脏"数据,需要写回? */
    boolean b_s_open;     /* is backing-store data valid? 有没有恢复的数据 */
    jvirt_barray_ptr next;    /* link to next virtual barray control block 下一块数据块 */
    backing_store_info b_s_info;  /* System-dependent control info 恢复数据的内容 */
};

typedef struct error_mgr_t * error_ptr;
struct error_mgr_t {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};
METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
    error_ptr myerr = (error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

PyObject *getdct(PyObject *self, PyObject *args)
{
    const char * filename = "lfs.jpg";
    FILE *infile = NULL;
    if (NULL == (infile=fopen(filename, "rb"))) {
        fprintf(stderr, "Can't open %s\n", filename);
        return NULL;
    }else {
        printf("file: %s opened.\n", filename);
    }

    struct jpeg_decompress_struct cinfo;
    struct error_mgr_t jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return NULL;
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);

    printf("Width:%4d, Height:%4d\n", cinfo.image_width, cinfo.image_height );
    printf("Components:%2d\n", cinfo.num_components);

    PyObject *dict = PyDict_New();
    jvirt_barray_ptr *coef_arrays = jpeg_read_coefficients(&cinfo);
    PyObject *component, *coef;
    for (int ci=0; ci!=cinfo.num_components; ++ci) {
        // 创建一个新的列表, 存储一个颜色分量所有DCT块
        jvirt_barray_ptr com_coef_array = coef_arrays[ci];
        JBLOCKARRAY block_array = com_coef_array->mem_buffer;
        
        int blocknum = (com_coef_array->rows_in_mem)*(com_coef_array->blocksperrow);
        component = PyList_New(blocknum);
        printf("Ready for one component.\n");

        for (int row=0; row!=com_coef_array->rows_in_mem; ++row) {
            for (int col=0; col!=com_coef_array->blocksperrow; ++col) {
                // 创建一个列表, 存储64个DCT分量
                coef = PyList_New(64);
                JCOEF *pcoef = block_array[row][col];
                for (int i=0; i!=DCTSIZE2; ++i) {
                    PyList_SetItem(coef, i, Py_BuildValue("i", pcoef[i]));
                }
                PyList_SetItem(component, row*(com_coef_array->blocksperrow) + col, coef);
                // PyList_Append(component, coef);
                printf("len(component) : %d\n", row*(com_coef_array->blocksperrow) + col);
            }
        }
        printf("Read one component.\n");

        // 添加到返回的字典中
        switch (cinfo.jpeg_color_space)
        {
        case JCS_GRAYSCALE:
            PyDict_SetItem(dict, Py_BuildValue("s", "Grey"), component);
            break;
        case JCS_RGB:
            switch (ci)
            {
            case 0:
                PyDict_SetItem(dict, Py_BuildValue("s", "R"), component);
                break;
            case 1:
                PyDict_SetItem(dict, Py_BuildValue("s", "G"), component);
                break;
            case 2:
                PyDict_SetItem(dict, Py_BuildValue("s", "B"), component);
                break;

            default:
                assert(0);
                break;
            }
            break;
        case JCS_YCbCr:
            switch (ci)
            {
            case 0:
                PyDict_SetItem(dict, Py_BuildValue("s", "Y"), component);
                break;
            case 1:
                PyDict_SetItem(dict, Py_BuildValue("s", "Cb"), component);
                break;
            case 2:
                PyDict_SetItem(dict, Py_BuildValue("s", "Cr"), component);
                break;

            default:
                assert(0);
                break;
            }
            break;

        default:
            assert(0);
            break;
        }
        printf("One Components Added.\n");
    }

    return dict;
}
