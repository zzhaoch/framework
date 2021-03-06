#include "sub_state_vector_observer.h"

using namespace FullPhysics;
using namespace blitz;

void SubStateVectorObserver::notify_update(const StateVector& Sv)
{
    if(Sv.state().rows() < pstart + plen) {
        std::stringstream err_msg;
        err_msg << "StateVector is of size: "
                << Sv.state().rows()
                << " not the expected size: "
                << (pstart + plen);
        throw Exception(err_msg.str());
    }

    if(pstart < 0) {
        throw Exception("pstart < 0");
    }

    sv_full.reference(Sv.state_with_derivative());
    sv_cov_full.reference(Sv.state_covariance());

    if(plen > 0) {
        blitz::Range rsub(pstart, pstart + plen - 1);
        sv_sub.reference(sv_full(rsub));
        sv_cov_sub.reference(Array<double, 2>(sv_cov_full(rsub, rsub)));
    }

    // So that anything registered as a SubStateVector observer
    // gets notified even if it doesn't contain any SV elements
    update_sub_state(sv_sub, sv_cov_sub);
}

void SubStateVectorObserver::mark_used(const StateVector& UNUSED(Sv),
                                       blitz::Array<bool, 1>& Used) const
{
    if(Used.rows() < pstart + plen) {
        throw Exception("StateVector not the expected size");
    }

    if(pstart < 0) {
        throw Exception("pstart < 0");
    }

    if(plen > 0) {
        Array<bool, 1> used_sub(Used(blitz::Range(pstart, pstart + plen - 1)));
        mark_used_sub(used_sub);
    }
}

void SubStateVectorObserver::state_vector_name(const StateVector& UNUSED(Sv),
        blitz::Array<std::string, 1>& Sv_name) const
{
    if(Sv_name.rows() < pstart + plen) {
        throw Exception("StateVector not the expected size");
    }

    if(pstart < 0) {
        throw Exception("pstart < 0");
    }

    if(plen > 0) {
        Array<std::string, 1> sv_name_sub(Sv_name(blitz::Range(pstart,
                                          pstart + plen - 1)));
        state_vector_name_sub(sv_name_sub);
    }
}

//-----------------------------------------------------------------------
/// Take the given number of state vector parameters. We determine
/// where the starting point to use is when we attach to the state
/// vector.
///
/// Note that it is perfectly legal for Plen to be 0, that just means
/// we don't have any parameters. This is a useful edge case that we
/// support.
//-----------------------------------------------------------------------

void SubStateVectorObserver::state_vector_observer_initialize(int Plen)
{
    range_min_check(Plen, 0);
    plen = Plen;
}
