// Global tmp_mpint. For performance reasons.

static mpz_t eagerFVs_mpint_tmp;
static gboolean eagerFVs_mpint_tmp_initialised = FALSE;

static void Init_eagerFVs_mpint_tmp (void)
{
    if (!eagerFVs_mpint_tmp_initialised)
    {
        mpz_init (eagerFVs_mpint_tmp);
        eagerFVs_mpint_tmp_initialised = TRUE;
    }
}

/****************************************************/

void Slice_and_COPY_DATA_FROM_TO (struct comp *c, int fromPort, int toPort, struct BitField *read_ports_bit_fields, int width)
{
    // COPY_DATA_FROM_TO (fromPort, toPort);
    // Slice the data to the correct bit field
    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        mpz_tdiv_q_2exp (eagerFVs_mpint_tmp, (mpz_ptr) GET_PORT_DATA (fromPort), read_ports_bit_fields[toPort - 3].low_index);
        mpz_tdiv_r_2exp (eagerFVs_mpint_tmp, eagerFVs_mpint_tmp, read_ports_bit_fields[toPort - 3].width);
        if (read_ports_bit_fields[toPort - 3].is_mpint)
        {
            SET_PORT_DATA (toPort, (long) eagerFVs_mpint_tmp);
        } else
        {
            SET_PORT_DATA (toPort, mpz_get_ui (eagerFVs_mpint_tmp));
        }
    } else
    {
        int data = GET_PORT_DATA (fromPort);
        int width = read_ports_bit_fields[toPort - 3].width;
        if (width != (sizeof (int) * 8))
            data = (data >> read_ports_bit_fields[toPort - 3].low_index) & ((1 << width) - 1);
        else
            data = (data >> read_ports_bit_fields[toPort - 3].low_index);

        SET_PORT_DATA (toPort, data);
    }
}

/****************************************************/

struct BrzPassiveEagerFalseVariable_
{
    STRUCT_COMP_BASE;

    int width;
    int nb_read_ports;
    struct BitField *read_ports_bit_fields;
    gboolean has_activate_requp_arrived; // flag for correctly handling the CallDemuxPush behaviour

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER

    int is_data_available;
    int *is_read_pending;       // array of booleans to indicate for each read port if a read is waiting to be processed (but blocked by the fact that the data hasn't arrived yet)
    int is_activate_rtz_pending; // boolean to indicate if the activate down is waiting to be processed (but blocked by the fact that the data hasn't arrived yet. This can happen when there is no read port)
    int is_activate_reqdown_pending;
    int is_data_reqdown_pending;

#else

    // Same as above, but instead of booleans, we use unsigned long longs that contain the timestamp of the event

    unsigned long long is_data_available;
    unsigned long long *is_read_pending;
    unsigned long long is_activate_rtz_pending;
    unsigned long long is_activate_reqdown_pending;
    unsigned long long is_data_reqdown_pending;

#endif
};

void BrzPassiveEagerFalseVariable_requp (struct comp *comp, int port)
{
    struct BrzPassiveEagerFalseVariable_ *c = (struct BrzPassiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
        c->has_activate_requp_arrived = TRUE;
        SEND_REQUP (2);
        break;

    case 1:                    // write port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_data_available = 1;
#else
        c->is_data_available = current_real_time;
#endif
        // Check if any read port is waiting for the data
        int i;

        for (i = 0; i < c->nb_read_ports; i++)
            if (c->is_read_pending[i])
            {
                Slice_and_COPY_DATA_FROM_TO (comp, 1, i + 3, c->read_ports_bit_fields, c->width);

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
                SEND_DATAON_DELAYED (i + 3, 0);
                SEND_ACKUP_DELAYED (i + 3, 1);
#else
                unsigned long long saved_current_real_time = current_real_time;

                current_real_time = MAX (c->is_read_pending[i], current_real_time);
                SEND_DATAON_DELAYED (i + 3, 0);
                SEND_ACKUP_DELAYED (i + 3, 1);
                current_real_time = saved_current_real_time;
#endif

                c->is_read_pending[i] = 0;
            }
        // Check if the activate down is waiting for the data
        if (c->is_activate_rtz_pending)
        {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_ACKUP (0);
            SEND_ACKUP (1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_activate_rtz_pending, current_real_time);
            SEND_ACKUP (0);
            SEND_ACKUP (1);
            current_real_time = saved_current_real_time;
#endif

            c->is_activate_rtz_pending = 0;
        }
        break;

    default:                   // one of the read ports
        if (c->is_data_available)
        {
            Slice_and_COPY_DATA_FROM_TO (comp, 1, port, c->read_ports_bit_fields, c->width);

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_DATAON_DELAYED (port, 0);
            SEND_ACKUP_DELAYED (port, 1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_data_available, current_real_time);
            SEND_DATAON_DELAYED (port, 0);
            SEND_ACKUP_DELAYED (port, 1);
            current_real_time = saved_current_real_time;
#endif
        } else
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            c->is_read_pending[port - 3] = 1;
#else
            c->is_read_pending[port - 3] = current_real_time;
#endif
        break;
    }
}
void BrzPassiveEagerFalseVariable_ackup (struct comp *comp, int port)
{
    struct BrzPassiveEagerFalseVariable_ *c = (struct BrzPassiveEagerFalseVariable_ *) comp;

    SEND_REQDOWN (2);
}

void BrzPassiveEagerFalseVariable_reqdown (struct comp *comp, int port)
{
    struct BrzPassiveEagerFalseVariable_ *c = (struct BrzPassiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_activate_reqdown_pending = 1;
#else
        c->is_activate_reqdown_pending = current_real_time;
#endif
        break;

    case 1:                    // write port
        c->is_data_available = 0;
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_data_reqdown_pending = 1;
#else
        c->is_data_reqdown_pending = current_real_time;
#endif
        break;

    default:                   // any read port
        SEND_ACKDOWN_DELAYED (port, 0);
        SEND_DATAOFF_DELAYED (port, 1);
        break;
    }

    if (c->is_data_reqdown_pending && (c->is_activate_reqdown_pending || !c->has_activate_requp_arrived))
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        if (c->has_activate_requp_arrived)
        {                       // Correctly handling the CallDemuxPush behaviour
            SEND_ACKDOWN (0);
        }
        SEND_ACKDOWN (1);
#else
        unsigned long long saved_current_real_time = current_real_time;

        current_real_time = MAX (c->is_activate_reqdown_pending, c->is_data_reqdown_pending);
        if (c->has_activate_requp_arrived)
        {                       // Correctly handling the CallDemuxPush behaviour
            SEND_ACKDOWN (0);
        }
        SEND_ACKDOWN (1);
        current_real_time = saved_current_real_time;
#endif
        c->has_activate_requp_arrived = FALSE;
        c->is_activate_reqdown_pending = 0;
        c->is_data_reqdown_pending = 0;
    }
}
void BrzPassiveEagerFalseVariable_ackdown (struct comp *comp, int port)
{
    struct BrzPassiveEagerFalseVariable_ *c = (struct BrzPassiveEagerFalseVariable_ *) comp;

    if (c->is_data_available)
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        SEND_ACKUP (0);
        SEND_ACKUP (1);
#else
        unsigned long long saved_current_real_time = current_real_time;

        current_real_time = MAX (c->is_data_available, current_real_time);
        SEND_ACKUP (0);
        SEND_ACKUP (1);
        current_real_time = saved_current_real_time;
#endif
    } else
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_activate_rtz_pending = 1;
#else
        c->is_activate_rtz_pending = current_real_time;
#endif
    }
}

//void BrzPassiveEagerFalseVariable (int num, int width, int nb_read_ports, int act, int in0, int commandAct, ...)
void BrzPassiveEagerFalseVariable (int num, char *args[])
{
    int width = atoi (args[0]);
    int nb_read_ports = atoi (args[1]);
    char *readPortsSpecString = (char *) args[2];
    int i;

    if (nb_read_ports == 0)
        g_warning ("PassiveEagerFalseVariable component with 0 read ports should be a NullAdapt component! please report this...");

    INIT_COMP (BrzPassiveEagerFalseVariable);
    INIT_COMP_PASSIVE_PORT (BrzPassiveEagerFalseVariable, 0, atoi (args[3]));
    INIT_COMP_PASSIVE_PORT (BrzPassiveEagerFalseVariable, 1, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzPassiveEagerFalseVariable, 2, atoi (args[5]));

    c->width = width;
    c->nb_read_ports = nb_read_ports;
    c->read_ports_bit_fields = decode_bit_fields (readPortsSpecString, nb_read_ports, width);
    c->has_activate_requp_arrived = FALSE;
    c->is_data_available = 0;
    c->is_activate_rtz_pending = 0;
    c->is_activate_reqdown_pending = 0;
    c->is_data_reqdown_pending = 0;

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->is_read_pending = g_new0 (int, nb_read_ports);
#else
    c->is_read_pending = g_new0 (unsigned long long, nb_read_ports);
#endif

    for (i = 0; i < nb_read_ports; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzPassiveEagerFalseVariable, i + 3, atoi (args[i + 6]));
        INIT_COMP_CONTROLLED_DATA_PORT (i + 3, atoi (args[i + 6]));
    }

    WRITE_DEBUG_INFO_COMP ("peFV", 3 + c->nb_read_ports);
    Init_eagerFVs_mpint_tmp ();
}

/****************************************************/

struct BrzActiveEagerFalseVariable_
{
    STRUCT_COMP_BASE;

    int width;
    int nb_read_ports;
    struct BitField *read_ports_bit_fields;

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER

    int is_data_available;
    int *is_read_pending;       // array of booleans to indicate for each read port if a read is waiting to be processed (but blocked by the fact that the data hasn't arrived yet)
    int is_activate_rtz_pending; // boolean to indicate if the activate down is waiting to be processed (but blocked by the fact that the data hasn't arrived yet. This can happen when there is no read port)
    int is_activate_reqdown_pending;
    int is_data_ackdown_pending;

#else

    // Same as above, but instead of booleans, we use unsigned long longs that contain the timestamp of the event

    unsigned long long is_data_available;
    unsigned long long *is_read_pending;
    unsigned long long is_activate_rtz_pending;
    unsigned long long is_activate_reqdown_pending;
    unsigned long long is_data_ackdown_pending;

#endif
};

void BrzActiveEagerFalseVariable_requp (struct comp *comp, int port)
{
    struct BrzActiveEagerFalseVariable_ *c = (struct BrzActiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
        SEND_REQUP (1);
        SEND_REQUP (2);
        break;

    default:                   // one of the read ports
        if (c->is_data_available)
        {
            Slice_and_COPY_DATA_FROM_TO (comp, 1, port, c->read_ports_bit_fields, c->width);

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_DATAON_DELAYED (port, 0);
            SEND_ACKUP_DELAYED (port, 1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_data_available, current_real_time);
            SEND_DATAON_DELAYED (port, 0);
            SEND_ACKUP_DELAYED (port, 1);
            current_real_time = saved_current_real_time;
#endif
        } else
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            c->is_read_pending[port - 3] = 1;
#else
            c->is_read_pending[port - 3] = current_real_time;
#endif
        break;
    }
}
void BrzActiveEagerFalseVariable_ackup (struct comp *comp, int port)
{
    struct BrzActiveEagerFalseVariable_ *c = (struct BrzActiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 1:                    // (data) write port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_data_available = 1;
#else
        c->is_data_available = current_real_time;
#endif
        // Check if any read port is waiting for the data
        int i;

        for (i = 0; i < c->nb_read_ports; i++)
            if (c->is_read_pending[i])
            {
                Slice_and_COPY_DATA_FROM_TO (comp, 1, i + 3, c->read_ports_bit_fields, c->width);

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
                SEND_DATAON_DELAYED (i + 3, 0);
                SEND_ACKUP_DELAYED (i + 3, 1);
#else
                unsigned long long saved_current_real_time = current_real_time;

                current_real_time = MAX (c->is_read_pending[i], current_real_time);
                SEND_DATAON_DELAYED (i + 3, 0);
                SEND_ACKUP_DELAYED (i + 3, 1);
                current_real_time = saved_current_real_time;
#endif

                c->is_read_pending[i] = 0;
            }
        // Check if the activate down is waiting for the data
        if (c->is_activate_rtz_pending)
        {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_ACKUP (0);
            SEND_REQDOWN (1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_activate_rtz_pending, current_real_time);
            SEND_ACKUP (0);
            SEND_REQDOWN (1);
            current_real_time = saved_current_real_time;
#endif

            c->is_activate_rtz_pending = 0;
        }
        break;

    case 2:                    // commandActivate port
        SEND_REQDOWN (2);
        break;
    }
}

void BrzActiveEagerFalseVariable_reqdown (struct comp *comp, int port)
{
    struct BrzActiveEagerFalseVariable_ *c = (struct BrzActiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        if (c->is_data_ackdown_pending)
        {
            SEND_ACKDOWN (0);
            c->is_data_ackdown_pending = 0;
        } else
            c->is_activate_reqdown_pending = 1;
#else
        if (c->is_data_ackdown_pending)
        {
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_data_ackdown_pending, current_real_time);
            SEND_ACKDOWN (0);
            current_real_time = saved_current_real_time;
            c->is_data_ackdown_pending = 0;
        } else
            c->is_activate_reqdown_pending = current_real_time;
#endif
        break;

    default:                   // any read port
        SEND_ACKDOWN_DELAYED (port, 0);
        SEND_DATAOFF_DELAYED (port, 1);
        break;
    }
}
void BrzActiveEagerFalseVariable_ackdown (struct comp *comp, int port)
{
    struct BrzActiveEagerFalseVariable_ *c = (struct BrzActiveEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 1:                    // (data) write port
        c->is_data_available = 0;

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        if (c->is_activate_reqdown_pending)
        {
            SEND_ACKDOWN (0);
            c->is_activate_reqdown_pending = 0;
        } else
            c->is_data_ackdown_pending = 1;
#else
        if (c->is_activate_reqdown_pending)
        {
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_activate_reqdown_pending, current_real_time);
            SEND_ACKDOWN (0);
            current_real_time = saved_current_real_time;
            c->is_activate_reqdown_pending = 0;
        } else
            c->is_data_ackdown_pending = current_real_time;
#endif
        break;

    case 2:                    // commandActivate port
        if (c->is_data_available)
        {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_ACKUP (0);
            SEND_REQDOWN (1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_data_available, current_real_time);
            SEND_ACKUP (0);
            SEND_REQDOWN (1);
            current_real_time = saved_current_real_time;
#endif
        } else
        {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            c->is_activate_rtz_pending = 1;
#else
            c->is_activate_rtz_pending = current_real_time;
#endif
        }
        break;
    }
}

//void BrzActiveEagerFalseVariable (int num, int width, int nb_read_ports, int act, int in0, int commandAct, ...)
void BrzActiveEagerFalseVariable (int num, char *args[])
{
    int width = atoi (args[0]);
    int nb_read_ports = atoi (args[1]);
    char *readPortsSpecString = (char *) args[2];
    int i;

    if (nb_read_ports == 0)
        g_warning ("PassiveEagerFalseVariable component with 0 read ports should be a NullAdapt component! please report this...");

    INIT_COMP (BrzActiveEagerFalseVariable);
    INIT_COMP_PASSIVE_PORT (BrzActiveEagerFalseVariable, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzActiveEagerFalseVariable, 1, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzActiveEagerFalseVariable, 2, atoi (args[5]));

    c->width = width;
    c->nb_read_ports = nb_read_ports;
    c->read_ports_bit_fields = decode_bit_fields (readPortsSpecString, nb_read_ports, width);
    c->is_data_available = 0;
    c->is_activate_rtz_pending = 0;
    c->is_activate_reqdown_pending = 0;
    c->is_data_ackdown_pending = 0;
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->is_read_pending = g_new0 (int, nb_read_ports);
#else
    c->is_read_pending = g_new0 (unsigned long long, nb_read_ports);
#endif

    for (i = 0; i < nb_read_ports; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzActiveEagerFalseVariable, i + 3, atoi (args[i + 6]));
        INIT_COMP_CONTROLLED_DATA_PORT (i + 3, atoi (args[i + 6]));
    }

    WRITE_DEBUG_INFO_COMP ("aeFV", 3 + c->nb_read_ports);
    Init_eagerFVs_mpint_tmp ();
}

/****************************************************/

struct BrzPassiveSyncEagerFalseVariable_
{
    STRUCT_COMP_BASE;

    int nb_read_ports;

#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER

    int is_data_available;
    int is_activate_rtz_pending; // boolean to indicate if the activate down is waiting to be processed (but blocked by the fact that the data hasn't arrived yet. This can happen when there is no read port)
    int is_activate_reqdown_pending;
    int is_data_reqdown_pending;

#else

    // Same as above, but instead of booleans, we use unsigned long longs that contain the timestamp of the event

    unsigned long long is_data_available;
    unsigned long long is_activate_rtz_pending;
    unsigned long long is_activate_reqdown_pending;
    unsigned long long is_data_reqdown_pending;

#endif
};

void BrzPassiveSyncEagerFalseVariable_requp (struct comp *comp, int port)
{
    struct BrzPassiveSyncEagerFalseVariable_ *c = (struct BrzPassiveSyncEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
        SEND_REQUP (2);
        break;

    case 1:                    // write port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_data_available = 1;
#else
        c->is_data_available = current_real_time;
#endif
        // Check if the activate down is waiting for the data
        if (c->is_activate_rtz_pending)
        {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
            SEND_ACKUP (0);
            SEND_ACKUP (1);
#else
            unsigned long long saved_current_real_time = current_real_time;

            current_real_time = MAX (c->is_activate_rtz_pending, current_real_time);
            SEND_ACKUP (0);
            SEND_ACKUP (1);
            current_real_time = saved_current_real_time;
#endif

            c->is_activate_rtz_pending = 0;
        }
        break;
    }
}
void BrzPassiveSyncEagerFalseVariable_ackup (struct comp *comp, int port)
{
    struct BrzPassiveSyncEagerFalseVariable_ *c = (struct BrzPassiveSyncEagerFalseVariable_ *) comp;

    SEND_REQDOWN (2);
}

void BrzPassiveSyncEagerFalseVariable_reqdown (struct comp *comp, int port)
{
    struct BrzPassiveSyncEagerFalseVariable_ *c = (struct BrzPassiveSyncEagerFalseVariable_ *) comp;

    switch (port)
    {
    case 0:                    // main activation port
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_activate_reqdown_pending = 1;
#else
        c->is_activate_reqdown_pending = current_real_time;
#endif
        break;

    case 1:                    // write port
        c->is_data_available = 0;
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_data_reqdown_pending = 1;
#else
        c->is_data_reqdown_pending = current_real_time;
#endif
        break;
    }

    if (c->is_activate_reqdown_pending && c->is_data_reqdown_pending)
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        SEND_ACKDOWN (0);
        SEND_ACKDOWN (1);
#else
        unsigned long long saved_current_real_time = current_real_time;

        current_real_time = MAX (c->is_activate_reqdown_pending, c->is_data_reqdown_pending);
        SEND_ACKDOWN (0);
        SEND_ACKDOWN (1);
        current_real_time = saved_current_real_time;
#endif
        c->is_activate_reqdown_pending = 0;
        c->is_data_reqdown_pending = 0;
    }
}
void BrzPassiveSyncEagerFalseVariable_ackdown (struct comp *comp, int port)
{
    struct BrzPassiveSyncEagerFalseVariable_ *c = (struct BrzPassiveSyncEagerFalseVariable_ *) comp;

    if (c->is_data_available)
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        SEND_ACKUP (0);
        SEND_ACKUP (1);
#else
        unsigned long long saved_current_real_time = current_real_time;

        current_real_time = MAX (c->is_data_available, current_real_time);
        SEND_ACKUP (0);
        SEND_ACKUP (1);
        current_real_time = saved_current_real_time;
#endif
    } else
    {
#ifndef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->is_activate_rtz_pending = 1;
#else
        c->is_activate_rtz_pending = current_real_time;
#endif
    }
}

//void BrzPassiveSyncEagerFalseVariable (int num, int act, int in0, int commandAct)
void BrzPassiveSyncEagerFalseVariable (int num, char *args[])
{
    INIT_COMP (BrzPassiveSyncEagerFalseVariable);
    INIT_COMP_PASSIVE_PORT (BrzPassiveSyncEagerFalseVariable, 0, atoi (args[0]));
    INIT_COMP_PASSIVE_PORT (BrzPassiveSyncEagerFalseVariable, 1, atoi (args[1]));
    INIT_COMP_ACTIVE_PORT (BrzPassiveSyncEagerFalseVariable, 2, atoi (args[2]));

    c->is_data_available = 0;
    c->is_activate_rtz_pending = 0;
    c->is_activate_reqdown_pending = 0;
    c->is_data_reqdown_pending = 0;

    WRITE_DEBUG_INFO_COMP ("pseFV", 3);
}

/****************************************************/

#define BrzPassiveEagerNullAdapt_ BrzPassiveEagerFalseVariable_

#define BrzPassiveEagerNullAdapt_requp BrzPassiveEagerFalseVariable_requp
#define BrzPassiveEagerNullAdapt_ackup BrzPassiveEagerFalseVariable_ackup
#define BrzPassiveEagerNullAdapt_reqdown BrzPassiveEagerFalseVariable_reqdown
#define BrzPassiveEagerNullAdapt_ackdown BrzPassiveEagerFalseVariable_ackdown

//void BrzPassiveEagerNullAdapt (int num, int width, int act, int in0, int commandAct)
void BrzPassiveEagerNullAdapt (int num, char *args[])
{
    INIT_COMP (BrzPassiveEagerNullAdapt);
    INIT_COMP_PASSIVE_PORT (BrzPassiveEagerNullAdapt, 0, atoi (args[1]));
    INIT_COMP_PASSIVE_PORT (BrzPassiveEagerNullAdapt, 1, atoi (args[2]));
    INIT_COMP_ACTIVE_PORT (BrzPassiveEagerNullAdapt, 2, atoi (args[3]));

    c->nb_read_ports = 0;
    c->is_data_available = 0;
    c->is_activate_rtz_pending = 0;
    c->is_activate_reqdown_pending = 0;
    c->is_data_reqdown_pending = 0;

    WRITE_DEBUG_INFO_COMP ("peNA", 3);
}

/****************************************************/

#define BrzActiveEagerNullAdapt_ BrzActiveEagerFalseVariable_

#define BrzActiveEagerNullAdapt_requp BrzActiveEagerFalseVariable_requp
#define BrzActiveEagerNullAdapt_ackup BrzActiveEagerFalseVariable_ackup
#define BrzActiveEagerNullAdapt_reqdown BrzActiveEagerFalseVariable_reqdown
#define BrzActiveEagerNullAdapt_ackdown BrzActiveEagerFalseVariable_ackdown

//void BrzActiveEagerNullAdapt (int num, int width, int act, int in0, int commandAct)
void BrzActiveEagerNullAdapt (int num, char *args[])
{
    INIT_COMP (BrzActiveEagerNullAdapt);
    INIT_COMP_PASSIVE_PORT (BrzActiveEagerNullAdapt, 0, atoi (args[1]));
    INIT_COMP_ACTIVE_PORT (BrzActiveEagerNullAdapt, 1, atoi (args[2]));
    INIT_COMP_ACTIVE_PORT (BrzActiveEagerNullAdapt, 2, atoi (args[3]));

    c->nb_read_ports = 0;
    c->is_data_available = 0;
    c->is_activate_rtz_pending = 0;
    c->is_activate_reqdown_pending = 0;
    c->is_data_ackdown_pending = 0;

    WRITE_DEBUG_INFO_COMP ("aeNA", 3);
}

/****************************************************/
