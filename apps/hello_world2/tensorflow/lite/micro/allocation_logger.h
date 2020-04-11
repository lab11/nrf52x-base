#ifndef NRF_ALLOC_LOGGER_H__
#define NRF_ALLOC_LOGGER_H__

#include <string>

namespace AllocationLogger
{

    // Used to hold information used during allocation calculations.
    struct AllocationInfo {
      char* name;
       size_t bytes;
      int first_created;
      int subgraph_index;
      bool needs_allocating;
    };

    static const int BUFFER_SIZE = 100;   // Must be a power of 2
    extern AllocationInfo allocations[BUFFER_SIZE];
    extern int index;

    inline void Log(std::string name, size_t bytes, int first_created, int subgraph_index, bool needs_allocating)
    {
        // Get next event index
        index++;
        AllocationInfo* current  = &(allocations[index]);
        
        char *cstr = new char[name.length() + 1];
        strcpy(cstr, name.c_str());
        current->name = cstr;
        current -> bytes = bytes ;
        current -> first_created = first_created;
        current -> subgraph_index = subgraph_index;
        current -> needs_allocating = needs_allocating;

        // Write an event at this index
        // Event* e = g_events + ((index) & (BUFFER_SIZE - 1));  // Wrap to buffer size
        // Event* e = &(g_events[index]);
        // e->msg = msg;
        // e->a = a;
        // e->b = b;
        // e->c = c;
        // e->d = d;
    }
}

#define ALOG(a, b, c, d, e) AllocationLogger::Log(a, b, c, d, e)

#endif //NRF_ALLOC_LOGGER_H__
