/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "mpiimpl.h"

/*
=== BEGIN_MPI_T_CVAR_INFO_BLOCK ===

cvars:
    - name        : MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTRA_ALGORITHM
      category    : COLLECTIVE
      type        : enum
      default     : auto
      class       : none
      verbosity   : MPI_T_VERBOSITY_USER_BASIC
      scope       : MPI_T_SCOPE_ALL_EQ
      description : |-
        Variable to select neighbor_alltoallv algorithm
        auto - Internal algorithm selection (can be overridden with MPIR_CVAR_COLL_SELECTION_TUNING_JSON_FILE)
        nb   - Force nb algorithm

    - name        : MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTER_ALGORITHM
      category    : COLLECTIVE
      type        : enum
      default     : auto
      class       : none
      verbosity   : MPI_T_VERBOSITY_USER_BASIC
      scope       : MPI_T_SCOPE_ALL_EQ
      description : |-
        Variable to select neighbor_alltoallv algorithm
        auto - Internal algorithm selection (can be overridden with MPIR_CVAR_COLL_SELECTION_TUNING_JSON_FILE)
        nb   - Force nb algorithm

    - name        : MPIR_CVAR_NEIGHBOR_ALLTOALLV_DEVICE_COLLECTIVE
      category    : COLLECTIVE
      type        : boolean
      default     : true
      class       : none
      verbosity   : MPI_T_VERBOSITY_USER_BASIC
      scope       : MPI_T_SCOPE_ALL_EQ
      description : >-
        This CVAR is only used when MPIR_CVAR_DEVICE_COLLECTIVES
        is set to "percoll".  If set to true, MPI_Neighbor_alltoallv will
        allow the device to override the MPIR-level collective
        algorithms.  The device might still call the MPIR-level
        algorithms manually.  If set to false, the device-override
        will be disabled.

=== END_MPI_T_CVAR_INFO_BLOCK ===
*/

/* any non-MPI functions go here, especially non-static ones */


int MPIR_Neighbor_alltoallv_allcomm_auto(const void *sendbuf, const int sendcounts[],
                                         const int sdispls[], MPI_Datatype sendtype, void *recvbuf,
                                         const int recvcounts[], const int rdispls[],
                                         MPI_Datatype recvtype, MPIR_Comm * comm_ptr)
{
    int mpi_errno = MPI_SUCCESS;

    MPIR_Csel_coll_sig_s coll_sig = {
        .coll_type = MPIR_CSEL_COLL_TYPE__NEIGHBOR_ALLTOALLV,
        .comm_ptr = comm_ptr,

        .u.neighbor_alltoallv.sendbuf = sendbuf,
        .u.neighbor_alltoallv.sendcounts = sendcounts,
        .u.neighbor_alltoallv.sdispls = sdispls,
        .u.neighbor_alltoallv.sendtype = sendtype,
        .u.neighbor_alltoallv.recvbuf = recvbuf,
        .u.neighbor_alltoallv.recvcounts = recvcounts,
        .u.neighbor_alltoallv.rdispls = rdispls,
        .u.neighbor_alltoallv.recvtype = recvtype,
    };

    MPII_Csel_container_s *cnt = MPIR_Csel_search(comm_ptr->csel_comm, coll_sig);
    MPIR_Assert(cnt);

    switch (cnt->id) {
        case MPII_CSEL_CONTAINER_TYPE__ALGORITHM__MPIR_Neighbor_alltoallv_allcomm_nb:
            mpi_errno =
                MPIR_Neighbor_alltoallv_allcomm_nb(sendbuf, sendcounts, sdispls, sendtype, recvbuf,
                                                   recvcounts, rdispls, recvtype, comm_ptr);
            break;

        default:
            MPIR_Assert(0);
    }

    return mpi_errno;
}

int MPIR_Neighbor_alltoallv_impl(const void *sendbuf, const int sendcounts[],
                                 const int sdispls[], MPI_Datatype sendtype,
                                 void *recvbuf, const int recvcounts[],
                                 const int rdispls[], MPI_Datatype recvtype, MPIR_Comm * comm_ptr)
{
    int mpi_errno = MPI_SUCCESS;

    if (comm_ptr->comm_kind == MPIR_COMM_KIND__INTRACOMM) {
        switch (MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTRA_ALGORITHM) {
            case MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTRA_ALGORITHM_nb:
                mpi_errno =
                    MPIR_Neighbor_alltoallv_allcomm_nb(sendbuf, sendcounts, sdispls, sendtype,
                                                       recvbuf, recvcounts, rdispls, recvtype,
                                                       comm_ptr);
                break;
            case MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTRA_ALGORITHM_auto:
                mpi_errno =
                    MPIR_Neighbor_alltoallv_allcomm_auto(sendbuf, sendcounts, sdispls, sendtype,
                                                         recvbuf, recvcounts, rdispls, recvtype,
                                                         comm_ptr);
                break;
            default:
                MPIR_Assert(0);
        }
    } else {
        switch (MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTER_ALGORITHM) {
            case MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTER_ALGORITHM_nb:
                mpi_errno =
                    MPIR_Neighbor_alltoallv_allcomm_nb(sendbuf, sendcounts, sdispls, sendtype,
                                                       recvbuf, recvcounts, rdispls, recvtype,
                                                       comm_ptr);
                break;
            case MPIR_CVAR_NEIGHBOR_ALLTOALLV_INTER_ALGORITHM_auto:
                mpi_errno =
                    MPIR_Neighbor_alltoallv_allcomm_auto(sendbuf, sendcounts, sdispls, sendtype,
                                                         recvbuf, recvcounts, rdispls, recvtype,
                                                         comm_ptr);
                break;
            default:
                MPIR_Assert(0);
        }
    }
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

int MPIR_Neighbor_alltoallv(const void *sendbuf, const int sendcounts[],
                            const int sdispls[], MPI_Datatype sendtype,
                            void *recvbuf, const int recvcounts[],
                            const int rdispls[], MPI_Datatype recvtype, MPIR_Comm * comm_ptr)
{
    int mpi_errno = MPI_SUCCESS;

    if ((MPIR_CVAR_DEVICE_COLLECTIVES == MPIR_CVAR_DEVICE_COLLECTIVES_all) ||
        ((MPIR_CVAR_DEVICE_COLLECTIVES == MPIR_CVAR_DEVICE_COLLECTIVES_percoll) &&
         MPIR_CVAR_BARRIER_DEVICE_COLLECTIVE)) {
        mpi_errno =
            MPID_Neighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts,
                                    rdispls, recvtype, comm_ptr);
    } else {
        mpi_errno = MPIR_Neighbor_alltoallv_impl(sendbuf, sendcounts, sdispls,
                                                 sendtype, recvbuf, recvcounts,
                                                 rdispls, recvtype, comm_ptr);
    }

    return mpi_errno;
}
