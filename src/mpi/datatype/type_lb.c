/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Type_lb */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Type_lb = PMPI_Type_lb
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Type_lb  MPI_Type_lb
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Type_lb as PMPI_Type_lb
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint * displacement)
    __attribute__ ((weak, alias("PMPI_Type_lb")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Type_lb
#define MPI_Type_lb PMPI_Type_lb
#endif

/*@
    MPI_Type_lb - Returns the lower-bound of a datatype

Input Parameters:
. datatype - datatype (handle)

Output Parameters:
. displacement - displacement of lower bound from origin,
                             in bytes (address integer)

.N Removed
The replacement for this routine is 'MPI_Type_Get_extent'.

.N SignalSafe

.N Fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_TYPE
.N MPI_ERR_ARG
@*/
int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint * displacement)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_TERSE_STATE_DECL(MPID_STATE_MPI_TYPE_LB);

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPIR_FUNC_TERSE_ENTER(MPID_STATE_MPI_TYPE_LB);

    /* Validate parameters, especially handles needing to be converted */
#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_DATATYPE(datatype, "datatype", mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif

    /* Validate parameters and objects (post conversion) */
#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_Datatype *datatype_ptr = NULL;

            /* Convert MPI object handles to object pointers */
            MPIR_Datatype_get_ptr(datatype, datatype_ptr);

            /* Validate datatype_ptr */
            MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
            if (mpi_errno)
                goto fn_fail;
            MPIR_ERRTEST_ARGNULL(displacement, "displacement", mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ...  */

    MPIR_Type_lb_impl(datatype, displacement);

    /* ... end of body of routine ... */

#ifdef HAVE_ERROR_CHECKING
  fn_exit:
#endif
    MPIR_FUNC_TERSE_EXIT(MPID_STATE_MPI_TYPE_LB);
    return mpi_errno;

    /* --BEGIN ERROR HANDLING-- */
#ifdef HAVE_ERROR_CHECKING
  fn_fail:
    {
        mpi_errno =
            MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER,
                                 "**mpi_type_lb", "**mpi_type_lb %D %p", datatype, displacement);
    }
    mpi_errno = MPIR_Err_return_comm(NULL, __func__, mpi_errno);
    goto fn_exit;
#endif
    /* --END ERROR HANDLING-- */
}
