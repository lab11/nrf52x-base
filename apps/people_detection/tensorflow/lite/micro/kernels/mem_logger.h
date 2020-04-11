#ifndef NRF_MEM_LOGGER_H__
#define NRF_MEM_LOGGER_H__


namespace MemLogger
{
    struct Event
    {
        const char* msg;  // Message string
        unsigned int a;      // A parameter which can mean anything you want
        unsigned int b;      // A parameter which can mean anything you want
        unsigned int c;      // A parameter which can mean anything you want
        unsigned int d;      // A parameter which can mean anything you want
        unsigned int f;
        unsigned int g;
        unsigned int h;
        unsigned int i;
        unsigned int j;
    };

    static const int BUFFER_SIZE = 100;   // Must be a power of 2
    extern Event g_events[BUFFER_SIZE];
    extern int index;

    inline void Log(const char* msg, unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int f, unsigned int g, unsigned int h, unsigned int i, unsigned int j)
    {
        // Get next event index
        index++;

        // Write an event at this index
        // Event* e = g_events + ((index) & (BUFFER_SIZE - 1));  // Wrap to buffer size
        Event* e = &(g_events[index]);
        e->msg = msg;
        e->a = a;
        e->b = b;
        e->c = c;
        e->d = d;
        e->f = f;
        e->g = g;
        e->h = h;
        e->i = i;
        e->j = j;

    }
}

#define LOG(m, a, b, c, d, f, g, h, i, j) MemLogger::Log(m, a, b, c, d, f, g, h, i, j)

#endif //NRF_MEM_LOGGER_H__
