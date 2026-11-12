// expected-error: slab Log must fit the token slot field
#include <uring/core/operation_slab.hpp>
static_assert(sizeof(uring::OperationSlab<std::uint64_t, 23>) > 0);
