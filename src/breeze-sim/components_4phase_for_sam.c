/****************************************************/

struct BrzFetchPush_
{
    STRUCT_COMP_BASE;

    int nb_reqs;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    unsigned long long other_req_timestamp;
#endif
};

void BrzFetchPush_requp (struct comp *comp, int port)
{
    struct BrzFetchPush_ *c = (struct BrzFetchPush_ *) comp;

    c->nb_reqs++;

    switch (c->nb_reqs)
    {
    case 1:
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->other_req_timestamp = current_real_time;
#endif
        break;

    case 2:
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        current_real_time = MAX (current_real_time, c->other_req_timestamp);
#endif
        COPY_DATA_FROM_TO (1, 2);
        SEND_DATAON_DELAYED (2, 0);
        SEND_REQUP_DELAYED (2, 1);
        c->nb_reqs = 0;
        break;
    }
}

void BrzFetchPush_ackup (struct comp *comp, int port)
{
    struct BrzFetchPush_ *c = (struct BrzFetchPush_ *) comp;

    SEND_REQDOWN_DELAYED (2, 1);
}

void BrzFetchPush_reqdown (struct comp *comp, int port)
{
    struct BrzFetchPush_ *c = (struct BrzFetchPush_ *) comp;

    SEND_ACKDOWN (port);
    if (port == 1)
    {
        SEND_DATAOFF_DELAYED (2, 1);
    }
}

void BrzFetchPush_ackdown (struct comp *comp, int port)
{
    struct BrzFetchPush_ *c = (struct BrzFetchPush_ *) comp;

    SEND_ACKUP (0);
    SEND_ACKUP (1);
}

//void BrzFetchPush (int num, int width, int pass, int in, int out, int reject)
void BrzFetchPush (int num, char *args[])
{
    INIT_COMP (BrzFetchPush);
    INIT_COMP_PASSIVE_PORT (BrzFetchPush, 0, atoi (args[1]));
    INIT_COMP_PASSIVE_PORT (BrzFetchPush, 1, atoi (args[2]));
    INIT_COMP_ACTIVE_PORT (BrzFetchPush, 2, atoi (args[3]));
    INIT_COMP_CONTROLLED_DATA_PORT (2, atoi (args[3]));

    c->nb_reqs = 0;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->other_req_timestamp = 0;
#endif

    WRITE_DEBUG_INFO_COMP ("->push", 3);
}

/****************************************************/

struct BrzFetchReject_
{
    STRUCT_COMP_BASE;

    enum
    { DATA_RECEIVED = 1, PASS_RECEIVED = 2, REJECT_RECEIVED = 4 } state;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    unsigned long long data_received_timestamp;
    unsigned long long pass_or_reject_received_timestamp;
#endif
};

void BrzFetchReject_requp (struct comp *comp, int port)
{
    struct BrzFetchReject_ *c = (struct BrzFetchReject_ *) comp;

    switch (port)
    {
    case 0:                    // "pass" port
        c->state |= PASS_RECEIVED;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->pass_or_reject_received_timestamp = current_real_time;
#endif
        break;

    case 1:                    // "data_in" port
        c->state |= DATA_RECEIVED;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->data_received_timestamp = current_real_time;
#endif
        break;

    case 3:                    // "reject" port
        c->state |= REJECT_RECEIVED;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->pass_or_reject_received_timestamp = current_real_time;
#endif
        break;
    }

    if (c->state == (DATA_RECEIVED | PASS_RECEIVED))
    {
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        current_real_time = MAX (c->data_received_timestamp, c->pass_or_reject_received_timestamp);
#endif
        COPY_DATA_FROM_TO (1, 2);
        SEND_DATAON_DELAYED (2, 0);
        SEND_REQUP_DELAYED (2, 1);
    } else if (c->state == (DATA_RECEIVED | REJECT_RECEIVED))
    {
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        current_real_time = MAX (c->data_received_timestamp, c->pass_or_reject_received_timestamp);
#endif
        SEND_ACKUP (1);
        SEND_ACKUP (3);
        c->state = 0;
    }
}

void BrzFetchReject_ackup (struct comp *comp, int port)
{
    struct BrzFetchReject_ *c = (struct BrzFetchReject_ *) comp;

    SEND_REQDOWN_DELAYED (2, 1);
}

void BrzFetchReject_reqdown (struct comp *comp, int port)
{
    struct BrzFetchReject_ *c = (struct BrzFetchReject_ *) comp;

    SEND_ACKDOWN (port);
    if (port == 1)
    {
        SEND_DATAOFF_DELAYED (2, 1);
    }
}

void BrzFetchReject_ackdown (struct comp *comp, int port)
{
    struct BrzFetchReject_ *c = (struct BrzFetchReject_ *) comp;

    SEND_ACKUP (0);
    SEND_ACKUP (1);
    c->state = 0;
}

//void BrzFetchReject (int num, int width, int pass, int in, int out, int reject)
void BrzFetchReject (int num, char *args[])
{
    INIT_COMP (BrzFetchReject);
    INIT_COMP_PASSIVE_PORT (BrzFetchReject, 0, atoi (args[1]));
    INIT_COMP_PASSIVE_PORT (BrzFetchReject, 1, atoi (args[2]));
    INIT_COMP_ACTIVE_PORT (BrzFetchReject, 2, atoi (args[3]));
    INIT_COMP_PASSIVE_PORT (BrzFetchReject, 3, atoi (args[4]));
    INIT_COMP_CONTROLLED_DATA_PORT (2, atoi (args[3]));

    c->state = 0;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->data_received_timestamp = 0;
    c->pass_or_reject_received_timestamp = 0;
#endif

    WRITE_DEBUG_INFO_COMP ("->rej", 4);
}

/****************************************************/
