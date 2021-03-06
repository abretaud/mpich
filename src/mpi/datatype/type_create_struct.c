/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Type_create_struct */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Type_create_struct = PMPI_Type_create_struct
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Type_create_struct  MPI_Type_create_struct
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Type_create_struct as PMPI_Type_create_struct
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Type_create_struct(int count, const int array_of_blocklengths[],
                           const MPI_Aint array_of_displacements[],
                           const MPI_Datatype array_of_types[], MPI_Datatype * newtype)
    __attribute__ ((weak, alias("PMPI_Type_create_struct")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Type_create_struct
#define MPI_Type_create_struct PMPI_Type_create_struct
#endif

/*@
   MPI_Type_create_struct - Create an MPI datatype from a general set of
   datatypes, displacements, and block sizes

Input Parameters:
+ count - number of blocks (integer) --- also number of entries in arrays
  array_of_types, array_of_displacements and array_of_blocklengths
. array_of_blocklengths - number of elements in each block (array of integer)
. array_of_displacements - byte displacement of each block (array of address integer)
- array_of_types - type of elements in each block (array of handles to
  datatype objects)

Output Parameters:
. newtype - new datatype (handle)

.N ThreadSafe

.N Fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_ARG
.N MPI_ERR_TYPE
@*/
int MPI_Type_create_struct(int count,
                           const int array_of_blocklengths[],
                           const MPI_Aint array_of_displacements[],
                           const MPI_Datatype array_of_types[], MPI_Datatype * newtype)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_TERSE_STATE_DECL(MPID_STATE_MPI_TYPE_CREATE_STRUCT);

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER(MPID_STATE_MPI_TYPE_CREATE_STRUCT);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            int j;
            MPIR_Datatype *datatype_ptr = NULL;

            MPIR_ERRTEST_COUNT(count, mpi_errno);

            if (count > 0) {
                MPIR_ERRTEST_ARGNULL(array_of_blocklengths, "blocklens", mpi_errno);
                MPIR_ERRTEST_ARGNULL(array_of_displacements, "indices", mpi_errno);
                MPIR_ERRTEST_ARGNULL(array_of_types, "types", mpi_errno);
            }

            for (j = 0; j < count; j++) {
                MPIR_ERRTEST_ARGNEG(array_of_blocklengths[j], "blocklen", mpi_errno);
                MPIR_ERRTEST_DATATYPE(array_of_types[j], "datatype[j]", mpi_errno);

                if (array_of_types[j] != MPI_DATATYPE_NULL && !HANDLE_IS_BUILTIN(array_of_types[j])) {
                    MPIR_Datatype_get_ptr(array_of_types[j], datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno != MPI_SUCCESS)
                        goto fn_fail;
                }
            }
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ... */

    mpi_errno = MPIR_Type_create_struct_impl(count, array_of_blocklengths, array_of_displacements,
                                             array_of_types, newtype);
    if (mpi_errno)
        goto fn_fail;

    /* ... end of body of routine ... */

  fn_exit:
    MPIR_FUNC_TERSE_EXIT(MPID_STATE_MPI_TYPE_CREATE_STRUCT);
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLING-- */
#ifdef HAVE_ERROR_CHECKING
    {
        mpi_errno =
            MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER,
                                 "**mpi_type_create_struct",
                                 "**mpi_type_create_struct %d %p %p %p %p", count,
                                 array_of_blocklengths, array_of_displacements, array_of_types,
                                 newtype);
    }
#endif
    mpi_errno = MPIR_Err_return_comm(NULL, __func__, mpi_errno);
    goto fn_exit;
    /* --END ERROR HANDLING-- */
}
