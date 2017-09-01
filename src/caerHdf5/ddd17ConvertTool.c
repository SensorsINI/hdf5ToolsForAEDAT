/************************************************************

  This example shows how to read variable-length datatypes 
  from the AEDAT3.1 hdf5 files from DDD-17. The part rows of
  the file are displayed on the screen.

  This file is intended for use with HDF5 Library version 1.8

  Author: Min Liu <minliu@ini.uzh.ch>

 ***e********************************************************/

#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTFILE       "convert_result.hdf5"
#define DATASET         "/dvs/data"
#define CHUNK0          4096
#define CHUNK1          8

int
main (int argc, char **argv)
{
    hid_t       file, filetype, memtype, space, dset;
                                    /* Handles */
    hid_t       group, subgroup, gcpl, dcpl, mspace;        /* Handles */
    ssize_t     size;                               /* Size of name */
    herr_t      status;
    H5G_info_t  ginfo;
    hvl_t       *rdata;             /* Pointer to vlen structures */
    char        *wdata;           /* Pointer of extended write data */
    hsize_t     oriDims[2], 
                dims[2] = {50, 2};
    hsize_t     maxdims[2] = {0, 0},
                chunk[2] = {CHUNK0, CHUNK1},
                extdims[2] = {0, 0};
    unsigned char         *ptr,
                ndims;
    hsize_t     i, j, k;
    char        *name;                              /* Output buffer */

    hsize_t     count[2];              /* size of the hyperslab in the file */
    hsize_t     offset[2];             /* hyperslab offset in the file */
    /*
     * Now we begin the read section of this example.  Here we assume
     * the dataset has the same name and rank, but can have any size.
     * Therefore we must allocate a new array to read in data using
     * malloc().
     */

    /*
     * Open file and dataset.
     */
    char * srcFilename = (argc < 2) ? "rec1498945830.hdf5" : argv[1];
    printf ("Input file name is %s.\n", srcFilename);
    file = H5Fopen (srcFilename, H5F_ACC_RDONLY, H5P_DEFAULT);
    dset = H5Dopen (file, DATASET, H5P_DEFAULT);

    /*
     * Get dataspace and allocate memory for array of vlen structures.
     * This does not actually allocate memory for the vlen data, that
     * will be done by the library.
     */
    space = H5Dget_space (dset);
    ndims = H5Sget_simple_extent_dims (space, oriDims, NULL);
    rdata = (hvl_t *) malloc (oriDims[0] * oriDims[1] * sizeof (hvl_t));

    /*
     * Create the memory datatype.
     */
    memtype = H5Tvlen_create (H5T_NATIVE_UCHAR);

    /*
     * Read the data.
     */
    status = H5Dread (dset, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);

    /*
     * Output the variable-length data to the screen.
    for (i=0; i<2; i++) {
        printf ("%s[%u]:\n  {","row",(unsigned int)i);
        for (k=0; k<oriDims[1]; k++) {
            printf(" |");
            ptr = rdata[i * oriDims[1] + k].p;
            for (j=0; j<rdata[i * oriDims[1] + k].len; j++) {
                printf (" %d", ptr[j]);
                if ( (j+1) < rdata[i * oriDims[1] + k].len )
                    printf (",");
            }
        }
        printf ("|}\n");
    }
     */

    /*
     * Close and release resources.  Note we must still free the
     * top-level pointer "rdata", as H5Dvlen_reclaim only frees the
     * actual variable-length data, and not the structures themselves.
     */
    // status = H5Dvlen_reclaim (memtype, space, H5P_DEFAULT, rdata);
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Tclose (memtype);
    status = H5Fclose (file);


    /*
     * Create a new file using the default properties.
     */
    char * dstFilename = (argc < 3) ? "convert_result.hdf5" : argv[2];
    printf ("Output file name is %s.\n", dstFilename);
    file = H5Fcreate (dstFilename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Create group creation property list and enable link creation
     * order tracking.  Attempting to track by creation order in a
     * group that does not have this property set will result in an
     * error.
     */
    gcpl = H5Pcreate (H5P_GROUP_CREATE);
    status = H5Pset_link_creation_order( gcpl, H5P_CRT_ORDER_TRACKED |
                H5P_CRT_ORDER_INDEXED );

    /*
     * Create primary group using the property list.
     */
    group = H5Gcreate (file, "dvs", H5P_DEFAULT, gcpl, H5P_DEFAULT);

    /*
     * Create subgroups in the primary group.  These will be tracked
     * by creation order.  Note that these groups do not have to have
     * the creation order tracking property set.
     */
    subgroup = H5Gcreate (group, "special", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Gclose (subgroup);
    subgroup = H5Gcreate (group, "polarity", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Gclose (subgroup);
    subgroup = H5Gcreate (group, "frame", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Gclose (subgroup);
    subgroup = H5Gcreate (group, "IMU6", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Gclose (subgroup);

    /*
     * Create dataspace with unlimited dimensions.
     */
    dims[0] = 0;
    dims[1] = 0;
    maxdims[0] = H5S_UNLIMITED;
    maxdims[1] = H5S_UNLIMITED;
    space = H5Screate_simple (2, dims, maxdims);

    /*
     * Create the dataset creation property list, and set the chunk
     * size.
     */
    dcpl = H5Pcreate (H5P_DATASET_CREATE);
    status = H5Pset_chunk (dcpl, 2, chunk);

    hid_t dset_spe_data = H5Dcreate (file, "/dvs/special/data", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_spe_systs = H5Dcreate (file, "/dvs/special/system_ts", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_pol_data = H5Dcreate (file, "/dvs/polarity/data", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_pol_systs = H5Dcreate (file, "/dvs/polarity/system_ts", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_frame_data = H5Dcreate (file, "/dvs/frame/data", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_frame_systs = H5Dcreate (file, "/dvs/frame/system_ts", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_IMU6_data = H5Dcreate (file, "/dvs/IMU6/data", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);

    hid_t dset_IMU6_systs = H5Dcreate (file, "/dvs/IMU6/system_ts", H5T_NATIVE_UINT8, space, H5P_DEFAULT, dcpl,
                H5P_DEFAULT);
    /* 
     * Iteratelly read the rows in the origin file.
     */
    int totalPktNum = 0, polPktNum = 0, spePktNum = 0, framePktNum = 0, IMU6PktNum = 0, nullPktNum = 0;
    int rowSize = 0;
    for (i=0; i<oriDims[0]; i++) {
        
        totalPktNum += 1;
        ptr = rdata[3*i + 1].p;
        if (ptr != NULL) {
            rowSize = ptr[4] + (ptr[5] << 8) + (ptr[6] << 16) + (ptr[7] << 24);
            if (ptr[0] == 1) {
                polPktNum += 1;
                dset = dset_pol_data;
                if (rowSize != 8) {
                    printf ("Something is wrong here, polarity events!\n");
                }
            }

            if (ptr[0] == 0) {
                spePktNum += 1;
                dset = dset_spe_data;
                if (rowSize != 8) {
                    printf ("Something is wrong here, special events!\n");
                }
            }

            if (ptr[0] == 2) {
                framePktNum += 1;
                dset = dset_frame_data;
                rowSize -= 36;
                if (rowSize % 128 == 0) {
                    rowSize = 128 * 2;
                }

                if (rowSize % 240 == 0) {
                    rowSize = 240 * 2;
                }

                if (rowSize % 346 == 0) {
                    rowSize = 346 * 2;
                }
            }

            if (ptr[0]  == 3) {
                IMU6PktNum += 1;
                dset = dset_IMU6_data;
            }
        } else {
            nullPktNum += 1;
            continue;
        }
        space = H5Dget_space (dset);    /* The output file dataspace handle */

        ndims = H5Sget_simple_extent_dims (space, dims, NULL);


        extdims[0] = (rdata[3*i + 2].len)/rowSize;
        extdims[1] = rowSize;
        ptr = rdata[3*i + 2].p;
        char **wdata2 = (char **) malloc(extdims[0] * extdims[1]);
		for (int i = 0; i < extdims[0]; i++) {
			wdata2[i] = (char *)malloc(extdims[1]);
		}

        /*
         * Copy packet data to the buffer for writing to the extended dataset.
         */
        for (j=0; j<extdims[0]; j++) {        
            for (k=0; k<extdims[1]; k++) {
                wdata2[j][k] = ptr[j * rowSize + k];
                // wdata2[j][k] = j * rowSize + k;
            }
        }

        /*
         * Create the memory space for writing
         */
        mspace = H5Screate_simple(2, extdims, NULL);

        extdims[0] += dims[0];

        /*
         * Extend the dataset.
         */
        status = H5Dset_extent (dset, extdims);

        /*
         * Retrieve the dataspace for the newly extended dataset.
         */
        space = H5Dget_space (dset);

        /* 
         * Define hyperslab in the dataset. 
         */
        offset[0] = dims[0];
        offset[1] = 0;
        count[0]  = extdims[0] - dims[0];
        count[1]  = rowSize;
        status = H5Sselect_hyperslab (space, H5S_SELECT_SET, offset, NULL, 
                count, NULL);

        status = H5Dwrite (dset, H5T_NATIVE_UINT8, mspace, space, H5P_DEFAULT, wdata2);
    }


    printf ("The total number of the total event packets is: %d.\n", totalPktNum);
    printf ("The total number of the polarity event packets is: %d.\n", polPktNum);
    printf ("The total number of the special event packets is: %d.\n", spePktNum);
    printf ("The total number of the frame event packets is: %d.\n", framePktNum);
    printf ("The total number of the IMU6 event packets is: %d.\n", IMU6PktNum);
    printf ("The total number of the null event packets is: %d.\n", nullPktNum);


    /*
     * Close and release resources.
     */
    free (rdata);
    status = H5Sclose (space);
    status = H5Sclose (mspace);
    status = H5Pclose (dcpl);
    status = H5Pclose (gcpl);
    status = H5Gclose (group);
    status = H5Fclose (file);


    return 0;
}

