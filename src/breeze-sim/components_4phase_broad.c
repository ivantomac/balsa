/*
  components_4phase_broad.c:

  Broad version of the breeze components whose implementation
  differ between the early and broad backends
*/


/****************************************************/

struct BrzBar_
{
    STRUCT_COMP_BASE;

    int guardCount;

    int nb_acks;
    unsigned long long max_event_time;
    int guardIndex;
};

void BrzBar_requp (struct comp *comp, int port)
{
    int i;
    struct BrzBar_ *c = (struct BrzBar_ *) comp;

    switch (port)
    {
    case 0:
        for (i = c->guardCount - 1; i >= 0; i--)
        {
            TRACE_NEW_THREAD (2 + i, 0);
            SEND_REQUP (2 + i);
        }
        c->nb_acks = 0;
        c->max_event_time = 0;
        break;
    case 1:
        if (c->guardIndex >= 0)
        {
            TRACE_FOLLOW_THREAD (2 + c->guardCount + c->guardIndex, 1);
            SEND_REQUP (2 + c->guardCount + c->guardIndex);
        } else
        {
            SEND_ACKUP (1);
        }
        break;
    }
}
void BrzBar_ackup (struct comp *comp, int port)
{
    int i;
    struct BrzBar_ *c = (struct BrzBar_ *) comp;

    if (port < (c->guardCount + 2))
    {
        c->nb_acks++;
        c->max_event_time = MAX (c->max_event_time, current_real_time);
        if (c->nb_acks == c->guardCount)
        {
            current_real_time = c->max_event_time;
            for (i = 0; i < c->guardCount; i++)
            {
                int cond;

                if (1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                    cond = GET_PORT_DATA (2 + i);
                else
                    cond = mpz_get_ui ((mpz_ptr) (GET_PORT_DATA (2 + i)));
                if (cond)
                {
                    c->guardIndex = i;
                    if (1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                    {
                        SET_PORT_DATA (0, 1);
                    } else
                        mpz_set_ui ((mpz_ptr) GET_PORT_DATA (0), 1);
                    goto s1;
                }
            }
            c->guardIndex = -1;
            if (1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            {
                SET_PORT_DATA (0, 0);
            } else
                mpz_set_ui ((mpz_ptr) GET_PORT_DATA (0), 0);
          s1:
            FAKE_SET_PORT_DATA_FOR_DEBUG (0);
            SEND_DATAON_DELAYED (0, 0);
            for (i = c->guardCount - 1; i >= 0; i--)
            {
                SEND_REQDOWN (2 + i);
            }
            c->nb_acks = 0;
        }
    } else
    {
        SEND_REQDOWN (port);
    }
}
void BrzBar_reqdown (struct comp *comp, int port)
{
    struct BrzBar_ *c = (struct BrzBar_ *) comp;

    if (port == 0)
    {
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (0, 1);
    } else
    {
        SEND_ACKDOWN (port);    //port == 1
    }
}
void BrzBar_ackdown (struct comp *comp, int port)
{
    struct BrzBar_ *c = (struct BrzBar_ *) comp;

    if (port < (c->guardCount + 2))
    {
        c->nb_acks++;
        c->max_event_time = MAX (c->max_event_time, current_real_time);
        if (c->nb_acks == c->guardCount)
        {
            current_real_time = c->max_event_time;
            SEND_ACKUP (0);
        }
    } else
    {
        SEND_ACKUP (1);
    }
}

//void BrzBar (int num, int guardCount, int inGuard, int inActivate, ...) // n outGuard, n outActivateOut
void BrzBar (int num, char *args[])
{
    int guardCount = atoi (args[0]);
    int i;

    INIT_COMP (BrzBar);
    INIT_COMP_PASSIVE_PORT (BrzBar, 0, atoi (args[1]));
    INIT_COMP_PASSIVE_PORT (BrzBar, 1, atoi (args[2]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[1]));

    for (i = 0; i < 2 * guardCount; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzBar, 2 + i, atoi (args[i + 3]));
    }

    if (1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
    } else
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
        mpz_set_ui ((mpz_ptr) GET_PORT_DATA (0), 0);
    }

    c->guardCount = guardCount;

    WRITE_DEBUG_INFO_COMP ("[]", 2 + 2 * guardCount);
}

/****************************************************/

struct BrzCallDemux_
{
    STRUCT_COMP_BASE;

    int nb_inputs;
    int active_port;
};

void BrzCallDemux_requp (struct comp *comp, int port)
{
    struct BrzCallDemux_ *c = (struct BrzCallDemux_ *) comp;

    c->active_port = port;
    TRACE_FOLLOW_THREAD (c->nb_inputs, port);
    SEND_REQUP (c->nb_inputs);
}

void BrzCallDemux_ackup (struct comp *comp, int port)
{
    struct BrzCallDemux_ *c = (struct BrzCallDemux_ *) comp;

    COPY_DATA_FROM_TO (port, c->active_port);
    SEND_DATAON_DELAYED (c->active_port, 0);
    SEND_ACKUP_DELAYED (c->active_port, 1);
}

void BrzCallDemux_reqdown (struct comp *comp, int port)
{
    struct BrzCallDemux_ *c = (struct BrzCallDemux_ *) comp;

    if (port != c->active_port)
    {
        fprintf (stderr, "BrzCallDemux_reqdown: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }

    SEND_REQDOWN_DELAYED (c->nb_inputs, BREEZE_CONTROL_DELAY);
}

void BrzCallDemux_ackdown (struct comp *comp, int port)
{
    struct BrzCallDemux_ *c = (struct BrzCallDemux_ *) comp;

    SEND_ACKDOWN_DELAYED (c->active_port, 0);
    SEND_DATAOFF_DELAYED (c->active_port, 1);
}

//void BrzCallDemux (int num, int width, int nb_inputs, ...) //int in1, int in2, ..., int out
void BrzCallDemux (int num, char *args[])
{
    int nb_inputs = atoi (args[1]);
    int i;

    INIT_COMP (BrzCallDemux);

    c->nb_inputs = nb_inputs;

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzCallDemux, i, atoi (args[i + 2]));
        INIT_COMP_CONTROLLED_DATA_PORT (i, atoi (args[i + 2]));
    }

    INIT_COMP_ACTIVE_PORT (BrzCallDemux, nb_inputs, atoi (args[nb_inputs + 2]));

    WRITE_DEBUG_INFO_COMP ("CallDemux", c->nb_inputs + 1);
}

/****************************************************/

struct BrzCallDemuxPush_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_acknowledges;
    unsigned long long max_event_time;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    struct globalEventStruct reordered_req_event;
    struct callback_ reordered_req_cb;
#endif
};

void BrzCallDemuxPush_requp (struct comp *comp, int port)
{
    int i;
    struct BrzCallDemuxPush_ *c = (struct BrzCallDemuxPush_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        COPY_DATA_FROM_TO (0, i + 1);
        TRACE_NEW_THREAD (i + 1, 0);
        SEND_DATAON_DELAYED (i + 1, 0);
        SEND_REQUP_DELAYED (i + 1, 1 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzCallDemuxPush_ackup (struct comp *comp, int port)
{
    struct BrzCallDemuxPush_ *c = (struct BrzCallDemuxPush_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == 1)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzCallDemuxPush2_reqdown (struct comp *comp, int port);
void BrzCallDemuxPush_reqdown (struct comp *comp, int port)
{
    struct BrzCallDemuxPush_ *c = (struct BrzCallDemuxPush_ *) comp;

    InsertReorderedEventAtCurrentTime (&c->reordered_req_event);
}

void BrzCallDemuxPush2_reqdown (struct comp *comp, int port)
#else //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzCallDemuxPush_reqdown (struct comp *comp, int port)
#endif                          //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
{
    int i;
    struct BrzCallDemuxPush_ *c = (struct BrzCallDemuxPush_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        SEND_REQDOWN_DELAYED (i + 1, 0);
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzCallDemuxPush_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzCallDemuxPush_ *c = (struct BrzCallDemuxPush_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);
        for (i = c->nb_outs - 1; i >= 0; i--)
        {
            SEND_DATAOFF_DELAYED (i + 1, 1);
        }
    }
}

//void BrzCallDemuxPush (int num, int width, int nb_outs, int in, ...)
void BrzCallDemuxPush (int num, char *args[])
{
    int nb_outs = atoi (args[1]);
    int i;

    fprintf (stderr, "A non-DI CallDemuxPush components is instantiated. The simulation can behave differently from the real circuit.\n");

    INIT_COMP (BrzCallDemuxPush);
    INIT_COMP_PASSIVE_PORT (BrzCallDemuxPush, 0, atoi (args[2]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzCallDemuxPush, i + 1, atoi (args[i + 3]));
        INIT_COMP_CONTROLLED_DATA_PORT (i + 1, atoi (args[i + 3]));
    }

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->reordered_req_event.cb = &c->reordered_req_cb;
    c->reordered_req_cb.fct = BrzCallDemuxPush2_reqdown;
    c->reordered_req_cb.comp = (struct comp *) c;
    c->reordered_req_cb.portnum = 0;
#endif

    WRITE_DEBUG_INFO_COMP ("CallDemuxPush", 1 + c->nb_outs);
}

/****************************************************/

struct BrzCallMux_
{
    STRUCT_COMP_BASE;

    int nb_inputs;
    int active_port;
};

void BrzCallMux_requp (struct comp *comp, int port)
{
    struct BrzCallMux_ *c = (struct BrzCallMux_ *) comp;

    c->active_port = port;
    COPY_DATA_FROM_TO (port, c->nb_inputs);
    TRACE_FOLLOW_THREAD (c->nb_inputs, port);
    SEND_DATAON_DELAYED (c->nb_inputs, 0);
    SEND_REQUP_DELAYED (c->nb_inputs, BREEZE_DATAPATH_DELAY);
}

void BrzCallMux_ackup (struct comp *comp, int port)
{
    struct BrzCallMux_ *c = (struct BrzCallMux_ *) comp;

    SEND_ACKUP (c->active_port);
}

void BrzCallMux_reqdown (struct comp *comp, int port)
{
    struct BrzCallMux_ *c = (struct BrzCallMux_ *) comp;

    if (port != c->active_port)
    {
        fprintf (stderr, "BrzCallMux_reqdown: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }

    SEND_REQDOWN_DELAYED (c->nb_inputs, BREEZE_CONTROL_DELAY);
}

void BrzCallMux_ackdown (struct comp *comp, int port)
{
    struct BrzCallMux_ *c = (struct BrzCallMux_ *) comp;

    SEND_ACKDOWN_DELAYED (c->active_port, 0);
    SEND_DATAOFF_DELAYED (c->nb_inputs, 1);
}

//void BrzCallMux (int num, int width, int nb_inputs, ...) //int in1, int in2, ..., int out
void BrzCallMux (int num, char *args[])
{
    int nb_inputs = atoi (args[1]);
    int i;

    INIT_COMP (BrzCallMux);

    c->nb_inputs = nb_inputs;

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzCallMux, i, atoi (args[i + 2]));
    }

    INIT_COMP_ACTIVE_PORT (BrzCallMux, nb_inputs, atoi (args[nb_inputs + 2]));
    INIT_COMP_CONTROLLED_DATA_PORT (nb_inputs, atoi (args[nb_inputs + 2]));

    WRITE_DEBUG_INFO_COMP ("CallMux", c->nb_inputs + 1);
}

/****************************************************/

struct BrzCase_
{
    STRUCT_COMP_BASE;

    int inputWidth;
    int outputCount;
    long *spec;

    int active_port;
    mpz_t mpint_tmp;
};

int BrzCase_decode_int (long *spec, int value)
{
    int i;

    for (i = 0; i < spec[0]; i++)
    {
        int valueTest = spec[3 * i + 1];
        int notdcs = spec[3 * i + 2];

        if ((value & notdcs) == valueTest)
            return spec[3 * i + 3];
    }
    //    fprintf(stderr,"BrzCase_decode => unknown case\n");
    return -1;
}

int BrzCase_decode_mpint (long *spec, mpz_ptr value, mpz_ptr mpint_tmp)
{
    int i;

    for (i = 0; i < spec[0]; i++)
    {
        mpz_ptr valueTest = (mpz_ptr) spec[3 * i + 1];
        mpz_ptr notdcs = (mpz_ptr) spec[3 * i + 2];
        int cmp;

        mpz_and (mpint_tmp, value, notdcs);
        cmp = mpz_cmp (mpint_tmp, valueTest);
        if (cmp == 0)
            return spec[3 * i + 3];
    }

    return -1;
}

void BrzCase_requp (struct comp *comp, int port)
{
    struct BrzCase_ *c = (struct BrzCase_ *) comp;

    if (c->inputWidth <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        c->active_port = BrzCase_decode_int (c->spec, GET_PORT_DATA (0));
    else
        c->active_port = BrzCase_decode_mpint (c->spec, (mpz_ptr) GET_PORT_DATA (0), c->mpint_tmp);

    if (c->active_port != -1)
    {
        TRACE_FOLLOW_THREAD (c->active_port + 1, 0);
        SEND_REQUP_DELAYED (c->active_port + 1, 2 * BREEZE_CONTROL_DELAY);
    } else
    {
        SEND_ACKUP (0);
    }
}
void BrzCase_ackup (struct comp *comp, int port)
{
    struct BrzCase_ *c = (struct BrzCase_ *) comp;

    SEND_ACKUP (0);
}

void BrzCase_reqdown (struct comp *comp, int port)
{
    struct BrzCase_ *c = (struct BrzCase_ *) comp;

    if (c->active_port != -1)
    {
        SEND_REQDOWN_DELAYED (c->active_port + 1, BREEZE_CONTROL_DELAY);
    } else
    {
        SEND_ACKDOWN (0);
    }
}
void BrzCase_ackdown (struct comp *comp, int port)
{
    struct BrzCase_ *c = (struct BrzCase_ *) comp;

    SEND_ACKDOWN (0);
}

long *BrzCase_parseSpecStr (char *specStr, int isMpint)
{
    int nbElts;
    char *ptr;
    long *result;

    //fprintf(stderr, "BrzCase_parseSpecStr: length(specStr)==%d\n", strlen(specStr));

    // example: specStr = "5,1,25,27,3m16,7,31,6,2m24,8m16,0,4m16;22;30;11,12,13,15,14,16,17,21,23;28,29,9"
    // result:
    /*
       int *caseSpec = (int*) malloc (26*sizeof(int)*3 + 1);
       caseSpec[0] = 26; //nb of cases
       caseSpec[1] = 5; //value
       caseSpec[2] = 0; //dcs
       caseSpec[3] = 0; //matchNo
       caseSpec[4] = 1; //value
       caseSpec[5] = 0; //dcs
       caseSpec[6] = 0; //matchNo
       caseSpec[7] = 25; //value
       caseSpec[8] = 0; //dcs
       caseSpec[9] = 0; //matchNo
       caseSpec[10] = 27; //value
       caseSpec[11] = 0; //dcs
       caseSpec[12] = 0; //matchNo
       caseSpec[13] = 3; //value
       caseSpec[14] = 16; //dcs
       caseSpec[15] = 0; //matchNo
       caseSpec[16] = 7; //value
       caseSpec[17] = 0; //dcs
       caseSpec[18] = 0; //matchNo
       caseSpec[19] = 31; //value
       caseSpec[20] = 0; //dcs
       caseSpec[21] = 0; //matchNo
       caseSpec[22] = 6; //value
       caseSpec[23] = 0; //dcs
       caseSpec[24] = 0; //matchNo
       caseSpec[25] = 2; //value
       caseSpec[26] = 24; //dcs
       caseSpec[27] = 0; //matchNo
       caseSpec[28] = 8; //value
       caseSpec[29] = 16; //dcs
       caseSpec[30] = 0; //matchNo
       caseSpec[31] = 0; //value
       caseSpec[32] = 0; //dcs
       caseSpec[33] = 0; //matchNo
       caseSpec[34] = 4; //value
       caseSpec[35] = 16; //dcs
       caseSpec[36] = 0; //matchNo
       caseSpec[37] = 22; //value
       caseSpec[38] = 0; //dcs
       caseSpec[39] = 1; //matchNo
       caseSpec[40] = 30; //value
       caseSpec[41] = 0; //dcs
       caseSpec[42] = 2; //matchNo
       caseSpec[43] = 11; //value
       caseSpec[44] = 0; //dcs
       caseSpec[45] = 3; //matchNo
       caseSpec[46] = 12; //value
       caseSpec[47] = 0; //dcs
       caseSpec[48] = 3; //matchNo
       caseSpec[49] = 13; //value
       caseSpec[50] = 0; //dcs
       caseSpec[51] = 3; //matchNo
       caseSpec[52] = 15; //value
       caseSpec[53] = 0; //dcs
       caseSpec[54] = 3; //matchNo
       caseSpec[55] = 14; //value
       caseSpec[56] = 0; //dcs
       caseSpec[57] = 3; //matchNo
       caseSpec[58] = 16; //value
       caseSpec[59] = 0; //dcs
       caseSpec[60] = 3; //matchNo
       caseSpec[61] = 17; //value
       caseSpec[62] = 0; //dcs
       caseSpec[63] = 3; //matchNo
       caseSpec[64] = 21; //value
       caseSpec[65] = 0; //dcs
       caseSpec[66] = 3; //matchNo
       caseSpec[67] = 23; //value
       caseSpec[68] = 0; //dcs
       caseSpec[69] = 3; //matchNo
       caseSpec[70] = 28; //value
       caseSpec[71] = 0; //dcs
       caseSpec[72] = 4; //matchNo
       caseSpec[73] = 29; //value
       caseSpec[74] = 0; //dcs
       caseSpec[75] = 4; //matchNo
       caseSpec[76] = 9; //value
       caseSpec[77] = 0; //dcs
       caseSpec[78] = 4; //matchNo
     */

    // Compute how many elements
    nbElts = 1;
    for (ptr = specStr; *ptr; ptr++)
        if (*ptr == ',' || *ptr == ';')
            nbElts++;

    result = (long *) malloc ((1 + nbElts * 3) * sizeof (long));
    result[0] = nbElts;

    // Process each element
    {
        int numElt = 0;
        int numCase = 0;
        long value;
        long notdcs;

        ptr = specStr;
        while (*ptr)
        {
            if (!isMpint)
                value = (long) atoll (ptr);
            else
                value = (long) atompint (ptr);

            while (*ptr && *ptr != 'm' && *ptr != ',' && *ptr != ';')
                ptr++;

            if (*ptr == 0 || *ptr != 'm')
            {
                if (!isMpint)
                    notdcs = ~0;
                else
                {
                    notdcs = (long) malloc (sizeof (mpz_t));
                    mpz_init_set_si ((mpz_ptr) notdcs, -1);
                }
            } else
            {
                if (!isMpint)
                    notdcs = ~atoi (ptr + 1);
                else
                {
                    notdcs = (long) atompint (ptr + 1);
                    mpz_com ((mpz_ptr) notdcs, (mpz_ptr) notdcs);
                }

                while (*ptr && *ptr != ',' && *ptr != ';')
                    ptr++;
            }

            result[1 + numElt * 3] = value;
            result[1 + numElt * 3 + 1] = notdcs;
            result[1 + numElt * 3 + 2] = numCase;
            numElt++;

            if (*ptr && *ptr == ';')
                numCase++;

            if (*ptr)
                ptr++;
        }
    }

    return result;
}

//void BrzCase (int num, int inputWidth, int outputCount, int *specification, int in, ...)
void BrzCase (int num, char *args[])
{
    int inputWidth = atoi (args[0]);
    int outputCount = atoi (args[1]);
    char *specStr = (char *) args[2];
    int i;

    INIT_COMP (BrzCase);
    INIT_COMP_PASSIVE_PORT (BrzCase, 0, atoi (args[3]));

    if (inputWidth > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        mpz_init (c->mpint_tmp);

    c->inputWidth = inputWidth;
    c->outputCount = outputCount;
    c->spec = BrzCase_parseSpecStr (specStr + 1, (inputWidth > (HOST_INTEGER_WIDTH_IN_BYTES * 8))); /* Skip leading " */

    for (i = 0; i < outputCount; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzCase, i + 1, atoi (args[i + 4]));
    }

    WRITE_DEBUG_INFO_COMP ("@", 1 + c->outputCount);
}

/****************************************************/

struct BrzCaseFetch_
{
    STRUCT_COMP_BASE;

    int indexWidth;
    int outputCount;
    long *spec;

    int active_port;
    mpz_t mpint_tmp;
};

void BrzCaseFetch_requp (struct comp *comp, int port)
{
    struct BrzCaseFetch_ *c = (struct BrzCaseFetch_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

void BrzCaseFetch_ackup (struct comp *comp, int port)
{
    struct BrzCaseFetch_ *c = (struct BrzCaseFetch_ *) comp;

    if (port == 1)
    {
        if (c->indexWidth <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            c->active_port = BrzCase_decode_int (c->spec, GET_PORT_DATA (1));
        else
            c->active_port = BrzCase_decode_mpint (c->spec, (mpz_ptr) GET_PORT_DATA (1), c->mpint_tmp);

        if (c->active_port != -1)
        {
            TRACE_FOLLOW_THREAD (c->active_port + 2, 0);
            SEND_REQUP_DELAYED (c->active_port + 2, 4 * BREEZE_CONTROL_DELAY);
        } else
        {
            fprintf (stderr, "CaseFetch warning: bad index (-1)\n");
            SEND_REQDOWN_DELAYED (1, 0);
        }
    } else
    {
        COPY_DATA_FROM_TO (port, 0);
        SEND_DATAON_DELAYED (0, 0);
        SEND_REQDOWN_DELAYED (1, 1);
    }
}
void BrzCaseFetch_reqdown (struct comp *comp, int port)
{
    struct BrzCaseFetch_ *c = (struct BrzCaseFetch_ *) comp;

    if (c->active_port != -1)
    {
        SEND_REQDOWN_DELAYED (c->active_port + 2, BREEZE_CONTROL_DELAY);
    } else
    {
        fprintf (stderr, "CaseFetch warning: bad index (-1)\n");
        SEND_ACKDOWN (0);
    }
}
void BrzCaseFetch_ackdown (struct comp *comp, int port)
{
    struct BrzCaseFetch_ *c = (struct BrzCaseFetch_ *) comp;

    if (port == 1)
    {
        SEND_ACKUP (0);
    } else
    {
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (0, 1);
    }
}

//void BrzCaseFetch (int num, int inputWidth, int indexWidth, int outputCount, int *spec, int in, int outIndex, ...)
void BrzCaseFetch (int num, char *args[])
{
    //    int inputWidth = atoi (args[0]);
    int indexWidth = atoi (args[1]);
    int outputCount = atoi (args[2]);
    char *specStr = (char *) args[3];
    int i;

    INIT_COMP (BrzCaseFetch);
    INIT_COMP_PASSIVE_PORT (BrzCaseFetch, 0, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzCaseFetch, 1, atoi (args[5]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[4]));

    if (indexWidth > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        mpz_init (c->mpint_tmp);

    c->indexWidth = indexWidth;
    c->outputCount = outputCount;
    c->spec = BrzCase_parseSpecStr (specStr + 1, (indexWidth > (HOST_INTEGER_WIDTH_IN_BYTES * 8))); /* Skip leading " */

    for (i = 0; i < outputCount; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzCaseFetch, i + 2, atoi (args[i + 6]));
    }

    WRITE_DEBUG_INFO_COMP ("@->", 2 + c->outputCount);
}

/****************************************************/

struct BrzEncode_
{
    STRUCT_COMP_BASE;

    int width;
    int nb_inputs;
    long *spec;

    int active_port;
};

void BrzEncode_requp (struct comp *comp, int port)
{
    struct BrzEncode_ *c = (struct BrzEncode_ *) comp;

    c->active_port = port;
    SET_PORT_DATA (c->nb_inputs, c->spec[port]);
    TRACE_FOLLOW_THREAD (c->nb_inputs, port);
    SEND_DATAON_DELAYED (c->nb_inputs, 0);
    SEND_REQUP_DELAYED (c->nb_inputs, BREEZE_DATAPATH_DELAY);
}

void BrzEncode_ackup (struct comp *comp, int port)
{
    struct BrzEncode_ *c = (struct BrzEncode_ *) comp;

    SEND_ACKUP (c->active_port);
}

void BrzEncode_reqdown (struct comp *comp, int port)
{
    struct BrzEncode_ *c = (struct BrzEncode_ *) comp;

    if (port != c->active_port)
    {
        fprintf (stderr, "BrzEncode_reqdown: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }

    SEND_REQDOWN_DELAYED (c->nb_inputs, BREEZE_CONTROL_DELAY);
}

void BrzEncode_ackdown (struct comp *comp, int port)
{
    struct BrzEncode_ *c = (struct BrzEncode_ *) comp;

    SEND_ACKDOWN_DELAYED (c->active_port, 0);
    SEND_DATAOFF_DELAYED (c->nb_inputs, 1);
}

long *BrzEncode_parseSpecStr (int width, char *specStr)
{
    int nbElts;
    char *ptr;
    long *result;

    // Compute how many elements
    nbElts = 1;
    for (ptr = specStr; *ptr; ptr++)
        if (*ptr == ';')
            nbElts++;

    result = (long *) malloc (nbElts * sizeof (long));

    // Process each element
    {
        int numElt = 0;

        ptr = specStr;
        while (*ptr)
        {
            if (width <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            {
                result[numElt] = atoi (ptr);
            } else
            {
                char *buftmp, *ptrtmp;

                ptrtmp = strchr (ptr, ';');
                if (ptrtmp)
                {
                    int length = ptrtmp - ptr;

                    buftmp = (char *) g_malloc (length + 1);
                    strncpy (buftmp, ptr, length);
                    buftmp[length] = 0;
                } else
                {
                    buftmp = (char *) g_malloc (strlen (ptr) + 1);
                    strcpy (buftmp, ptr);
                }
                result[numElt] = (long) malloc (sizeof (mpz_t));
                mpz_init ((mpz_ptr) result[numElt]);
                mpz_set_str ((mpz_ptr) result[numElt], buftmp, 10);
                g_free (buftmp);
            }
            numElt++;

            while (*ptr && *ptr != ';')
                ptr++;

            if (*ptr)
                ptr++;
        }
    }

    return result;
}

//void BrzEncode (int num, int outputWidth, int nb_inputs, int *spec, ...) //int in1, int in2, ..., int out
void BrzEncode (int num, char *args[])
{
    int outputWidth = atoi (args[0]);
    int nb_inputs = atoi (args[1]);
    char *specStr = (char *) args[2];
    int i;

    INIT_COMP (BrzEncode);

    c->width = outputWidth;
    c->nb_inputs = nb_inputs;
    c->spec = BrzEncode_parseSpecStr (outputWidth, specStr + 1); /* Skip leading " */

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzEncode, i, atoi (args[i + 3]));
    }

    INIT_COMP_ACTIVE_PORT (BrzEncode, nb_inputs, atoi (args[3 + nb_inputs]));
    INIT_COMP_CONTROLLED_DATA_PORT (nb_inputs, atoi (args[3 + nb_inputs]));

    WRITE_DEBUG_INFO_COMP ("Encode", c->nb_inputs + 1);
}

/****************************************************/

struct BrzFalseVariable_
{
    STRUCT_COMP_BASE;

    int width;
    int nb_read_ports;
    struct BitField *read_ports_bit_fields;
    int port1_is_requp;         // flag for correctly handling the CallDemuxPush behaviour

    mpz_t *mpint_tmp;
};

void BrzFalseVariable_requp (struct comp *comp, int port)
{
    struct BrzFalseVariable_ *c = (struct BrzFalseVariable_ *) comp;

    if (port == 0)
    {
        TRACE_FOLLOW_THREAD (1, 0);
        c->port1_is_requp = TRUE;
        SEND_REQUP (1);
    } else
    {
//        COPY_DATA_FROM_TO (0, port);
        // Slice the data to the correct bit field
        if (c->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_tdiv_q_2exp (c->mpint_tmp[port-2], (mpz_ptr) GET_PORT_DATA (0), c->read_ports_bit_fields[port - 2].low_index);
            mpz_tdiv_r_2exp (c->mpint_tmp[port-2], c->mpint_tmp[port-2], c->read_ports_bit_fields[port - 2].width);
            if (c->read_ports_bit_fields[port - 2].is_mpint)
            {
                SET_PORT_DATA (port, (long) c->mpint_tmp[port-2]);
            } else
            {
                SET_PORT_DATA (port, mpz_get_ui (c->mpint_tmp[port-2]));
            }
        } else
        {
            int data = GET_PORT_DATA (0);
            int width = c->read_ports_bit_fields[port - 2].width;
            if (width != (sizeof (int) * 8))
                data = (data >> c->read_ports_bit_fields[port - 2].low_index) & ((1 << width) - 1);
            else
                data = (data >> c->read_ports_bit_fields[port - 2].low_index);

            SET_PORT_DATA (port, data);
        }

        SEND_DATAON_DELAYED (port, 0);
        SEND_ACKUP_DELAYED (port, 1);
    }
}
void BrzFalseVariable_ackup (struct comp *comp, int port)
{
    struct BrzFalseVariable_ *c = (struct BrzFalseVariable_ *) comp;

    c->port1_is_requp = FALSE;
    SEND_REQDOWN (1);
}

void BrzFalseVariable_reqdown (struct comp *comp, int port)
{
    struct BrzFalseVariable_ *c = (struct BrzFalseVariable_ *) comp;

    if (port == 0)
    {
        // correctly handling the CallDemuxPush behaviour
        if (c->port1_is_requp)
        {
            SEND_REQDOWN (1);
        } else
        {
            SEND_ACKDOWN (0);
        }
    } else
    {
        SEND_ACKDOWN_DELAYED (port, 0);
        SEND_DATAOFF_DELAYED (port, 1);
    }
}
void BrzFalseVariable_ackdown (struct comp *comp, int port)
{
    struct BrzFalseVariable_ *c = (struct BrzFalseVariable_ *) comp;

    if (c->port1_is_requp)
    {                           // correctly handling the CallDemuxPush behaviour
        c->port1_is_requp = FALSE;
        SEND_ACKDOWN (0);
    } else
    {                           // normal case
        SEND_ACKUP (0);
    }
}

//void BrzFalseVariable (int num, int width, int nb_read_ports, int in0, int out, ...)
void BrzFalseVariable (int num, char *args[])
{
    int width = atoi (args[0]);
    int nb_read_ports = atoi (args[1]);
    char *readPortsSpecString = (char *) args[2];
    int i;

    INIT_COMP (BrzFalseVariable);
    INIT_COMP_PASSIVE_PORT (BrzFalseVariable, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzFalseVariable, 1, atoi (args[4]));

    c->width = width;
    c->nb_read_ports = nb_read_ports;
    c->read_ports_bit_fields = decode_bit_fields (readPortsSpecString, nb_read_ports, width);
    c->port1_is_requp = FALSE;

    if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        c->mpint_tmp = g_new (mpz_t, nb_read_ports);

    for (i = 0; i < nb_read_ports; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzFalseVariable, i + 2, atoi (args[i + 5]));
        INIT_COMP_CONTROLLED_DATA_PORT (i + 2, atoi (args[i + 5]));

        if (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            mpz_init (c->mpint_tmp[i]);
    }

    WRITE_DEBUG_INFO_COMP ("FV", 2 + c->nb_read_ports);
}

/****************************************************/

struct BrzNullAdapt_
{
    STRUCT_COMP_BASE;
};

void BrzNullAdapt_requp (struct comp *comp, int port)
{
    struct BrzNullAdapt_ *c = (struct BrzNullAdapt_ *) comp;

    TRACE_FOLLOW_THREAD (0, 1);
    SEND_REQUP (0);
}

void BrzNullAdapt_ackup (struct comp *comp, int port)
{
    struct BrzNullAdapt_ *c = (struct BrzNullAdapt_ *) comp;

    SEND_REQDOWN (0);
}

void BrzNullAdapt_reqdown (struct comp *comp, int port)
{
    struct BrzNullAdapt_ *c = (struct BrzNullAdapt_ *) comp;

    SEND_ACKDOWN (1);
}

void BrzNullAdapt_ackdown (struct comp *comp, int port)
{
    struct BrzNullAdapt_ *c = (struct BrzNullAdapt_ *) comp;

    SEND_ACKUP (1);
}

//void BrzNullAdapt (int num, int width_in, int out, int in)
void BrzNullAdapt (int num, char *args[])
{
    INIT_COMP (BrzNullAdapt);
    INIT_COMP_ACTIVE_PORT (BrzNullAdapt, 0, atoi (args[1]));
    INIT_COMP_PASSIVE_PORT (BrzNullAdapt, 1, atoi (args[2]));

    WRITE_DEBUG_INFO_COMP ("NullAdapt", 2);
}

/****************************************************/

struct BrzPassivatorPush_
{
    STRUCT_COMP_BASE;
    int nb_outputs;
    int nb_requps;
    int nb_reqdowns;
    unsigned long long max_event_time;
};

void BrzPassivatorPush_requp (struct comp *comp, int port)
{
    struct BrzPassivatorPush_ *c = (struct BrzPassivatorPush_ *) comp;

    c->nb_requps++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_requps == c->nb_outputs + 1)
    {
        int i;

        current_real_time = c->max_event_time;

        for (i = 0; i < c->nb_outputs; i++)
        {
            COPY_DATA_FROM_TO (c->nb_outputs, i);
            SEND_DATAON_DELAYED (i, 0);
        }
        for (i = c->nb_outputs; i >= 0; i--)
        {
            SEND_ACKUP_DELAYED (i, BREEZE_DATAPATH_DELAY
              /*+ i * BREEZE_CONTROL_DELAY */ );
        }
        c->nb_requps = 0;
        c->max_event_time = 0;
    }
}
void BrzPassivatorPush_ackup (struct comp *comp, int port)
{
    //    struct BrzPassivatorPush_ *c = (struct BrzPassivatorPush_ *) comp;

}
void BrzPassivatorPush_reqdown (struct comp *comp, int port)
{
    struct BrzPassivatorPush_ *c = (struct BrzPassivatorPush_ *) comp;

    // Handle the case where the PassivatorPush follows a CallDemuxPush, and is therefore able to receive
    // upstream reqdowns before having received all its requps
    if (c->nb_requps != 0)
    {
        SEND_ACKDOWN (port);
        c->nb_requps = 0;
        return;
    }

    c->nb_reqdowns++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqdowns == c->nb_outputs + 1)
    {
        int i;

        current_real_time = c->max_event_time;

        for (i = c->nb_outputs; i >= 0; i--)
        {
            SEND_ACKDOWN_DELAYED (i, 0 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
        }
        for (i = 0; i < c->nb_outputs; i++)
        {
            SEND_DATAOFF_DELAYED (i, 1);
        }
        c->nb_reqdowns = 0;
        c->max_event_time = 0;
    }
}
void BrzPassivatorPush_ackdown (struct comp *comp, int port)
{
    //    struct BrzPassivatorPush_ *c = (struct BrzPassivatorPush_ *) comp;
}

//void BrzPassivatorPush (int num, int width, int nb_outputs, ...)
void BrzPassivatorPush (int num, char *args[])
{
    int nb_outputs = atoi (args[1]);
    int i;

    INIT_COMP (BrzPassivatorPush);

    c->nb_outputs = nb_outputs;

    for (i = 0; i < nb_outputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzPassivatorPush, i, atoi (args[i + 2]));
        INIT_COMP_CONTROLLED_DATA_PORT (i, atoi (args[i + 2]));
    }

    INIT_COMP_PASSIVE_PORT (BrzPassivatorPush, nb_outputs, atoi (args[nb_outputs + 2]));

    c->nb_requps = 0;
    c->nb_reqdowns = 0;
    c->max_event_time = 0;

    WRITE_DEBUG_INFO_COMP ("PassivatorPush", 1 + c->nb_outputs);
}

/****************************************************/

struct BrzVariable_
{
    STRUCT_COMP_BASE;

    int width;
    int nb_read_ports;
    struct BitField *read_ports_bit_fields;
    long data;
    int isInitialised;
    char *variableName;

    mpz_t mpint_tmp;
};

void BrzVariable_requp (struct comp *comp, int port)
{
    printf ("BrzVariable_requp: This code should never be reached\n");
    CloseBreezeSimulation (2);
}
void BrzVariable_requp_int (struct comp *comp, int port)
{
    struct BrzVariable_ *c = (struct BrzVariable_ *) comp;

    if (port == 0)
    {
        c->isInitialised = 1;
        c->data = GET_PORT_DATA (0);
        SEND_ACKUP (0);
    } else
    {
        if (c->isInitialised == 0)
        {
            char *msg = g_strdup_printf ("Uninitialised variable read (0) `%s' ", c->variableName);

            ReportError (comp->chan[port], msg);
            g_free (msg);
            c->isInitialised = -1;
        }
        // Slice the data to the correct bit field
        int data = c->data;
        int width = c->read_ports_bit_fields[port - 1].width;
        if (width != (sizeof (int) * 8))
            data = (data >> c->read_ports_bit_fields[port - 1].low_index) & ((1 << width) - 1);
        else
            data = (data >> c->read_ports_bit_fields[port - 1].low_index);

        SET_PORT_DATA (port, data);

        SEND_DATAON_DELAYED (port, 0);
        SEND_ACKUP_DELAYED (port, 1);
    }
}
void BrzVariable_requp_mpint (struct comp *comp, int port)
{
    struct BrzVariable_ *c = (struct BrzVariable_ *) comp;

    if (port == 0)
    {
        c->isInitialised = 1;
        mpz_set ((mpz_ptr) c->data, (mpz_ptr) GET_PORT_DATA (0));
        /*
           fprintf (stderr,"variable set to ");
           mpzui (c->data);
           fprintf (stderr,"\n");
         */

        SEND_ACKUP (0);
    } else
    {
        if (c->isInitialised == 0)
        {
            char *msg = g_strdup_printf ("Uninitialised variable read (0) `%s' ", c->variableName);

            ReportError (comp->chan[port], msg);
            g_free (msg);
            c->isInitialised = -1;
        }
        // Slice the data to the correct bit field
        mpz_tdiv_q_2exp (c->mpint_tmp, (mpz_ptr) c->data, c->read_ports_bit_fields[port - 1].low_index);
        mpz_tdiv_r_2exp (c->mpint_tmp, c->mpint_tmp, c->read_ports_bit_fields[port - 1].width);
        if (c->read_ports_bit_fields[port - 1].is_mpint)
        {
            SET_PORT_DATA (port, (long) c->mpint_tmp);
        } else
        {
            SET_PORT_DATA (port, mpz_get_ui (c->mpint_tmp));
        }

        /*
           fprintf (stderr,"variable read1 to ");
           mpzui (c->data);
           fprintf (stderr,"\n");
           fprintf (stderr,"variable read2 to ");
           mpzui (c->chan[port]->data);
           fprintf (stderr,"\n");
         */
        SEND_DATAON_DELAYED (port, 0);
        SEND_ACKUP_DELAYED (port, 1);
    }
}
void BrzVariable_ackup (struct comp *comp, int port)
{
    //    struct BrzVariable_ *c = (struct BrzVariable_ *) comp;
}
void BrzVariable_reqdown (struct comp *comp, int port)
{
    struct BrzVariable_ *c = (struct BrzVariable_ *) comp;

    SEND_ACKDOWN_DELAYED (port, 0);

    if (port >= 1)
    {
        SEND_DATAOFF_DELAYED (port, 1);
    }
}

void BrzVariable_ackdown (struct comp *comp, int port)
{
    //    struct BrzVariable_ *c = (struct BrzVariable_ *) comp;
}

//void BrzVariable (int num, int width, int nb_read_ports, char *name, ...)
void BrzVariable (int num, char *args[])
{
    int width = atoi (args[0]);
    int nb_read_ports = atoi (args[1]);
    char *name = (char *) args[2];
    char *readPortsSpecString = (char *) args[3];
    int write = atoi (args[4]);
    int i;
    gboolean is_write_port_mpint = (width > (HOST_INTEGER_WIDTH_IN_BYTES * 8));

    if (readPortsSpecString[0] == '\"') // Remove the leading \"
        readPortsSpecString++;
    else
    {
        fprintf (stderr,
          "Warning! Your Breeze circuit has been compiled with a deprecated version of the Balsa compiler (The BrzVariable components have been extended). You should recompile your circuit to be able to simulate it.\n");
        exit (-2);
    }
    INIT_COMP (BrzVariable);
    INIT_COMP_PASSIVE_PORT (BrzVariable, 0, write);

    if (is_write_port_mpint)
    {
        c->data = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) c->data);
        mpz_set_ui ((mpz_ptr) c->data, 0);
        channel[write].requp.fct = BrzVariable_requp_mpint;
        mpz_init (c->mpint_tmp);
    } else
    {
        c->data = 0;
        channel[write].requp.fct = BrzVariable_requp_int;
    }

    c->width = width;
    c->nb_read_ports = nb_read_ports;
    c->read_ports_bit_fields = decode_bit_fields (readPortsSpecString, nb_read_ports, width);
    c->isInitialised = 0;
    c->variableName = g_strdup (name + 1); /* Skip leading " */

    for (i = 1; i <= nb_read_ports; i++)
    {
        int chan_num = atoi (args[i + 4]);

        INIT_COMP_PASSIVE_PORT (BrzVariable, i, chan_num);
        INIT_COMP_CONTROLLED_DATA_PORT (i, chan_num);

        if (is_write_port_mpint)
            channel[chan_num].requp.fct = BrzVariable_requp_mpint;
        else
            channel[chan_num].requp.fct = BrzVariable_requp_int;
    }

    WRITE_DEBUG_INFO_COMP (name, 1 + c->nb_read_ports);
}

/****************************************************/

struct BrzWhile_
{
    STRUCT_COMP_BASE;
    int guard;
};

void BrzWhile_requp (struct comp *comp, int port)
{
    struct BrzWhile_ *c = (struct BrzWhile_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    TRACE_FOLLOW_THREAD (2, 0);
    SEND_REQUP (1);
}

void BrzWhile_ackup (struct comp *comp, int port)
{
    struct BrzWhile_ *c = (struct BrzWhile_ *) comp;

    switch (port)
    {
    case 1:
        if (1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            c->guard = GET_PORT_DATA (1);
        else
            c->guard = mpz_get_ui ((mpz_ptr) GET_PORT_DATA (1));
        SEND_REQDOWN (1);
        break;
    case 2:
        SEND_REQDOWN (2);
        break;
    }
}
void BrzWhile_reqdown (struct comp *comp, int port)
{
    struct BrzWhile_ *c = (struct BrzWhile_ *) comp;

    SEND_ACKDOWN (0);
}

void BrzWhile_ackdown (struct comp *comp, int port)
{
    struct BrzWhile_ *c = (struct BrzWhile_ *) comp;

    switch (port)
    {
    case 1:
        if (c->guard)
        {
            SEND_REQUP (2);
        } else
        {
            SEND_ACKUP (0);
        }
        break;
    case 2:
        SEND_REQUP (1);
        break;
    }
}

//void BrzWhile (int num, int activate, int guard, int activateOut)
void BrzWhile (int num, char *args[])
{
    INIT_COMP (BrzWhile);
    INIT_COMP_PASSIVE_PORT (BrzWhile, 0, atoi (args[0]));
    INIT_COMP_ACTIVE_PORT (BrzWhile, 1, atoi (args[1]));
    INIT_COMP_ACTIVE_PORT (BrzWhile, 2, atoi (args[2]));

    WRITE_DEBUG_INFO_COMP ("DO", 3);
}

/****************************************************/

struct BrzArbiter_
{
    STRUCT_COMP_BASE;
    enum BrzArbiterState
    { idle1idle2, busy1idle2, busy1waiting2, busy2idle1, busy2waiting1 }
    state;
#ifdef DELAYS_WITH_ERROR
    GList *history;
#endif
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    struct globalEventStruct reordered_req_event[2];
    struct callback_ reordered_req_cb[2];
    unsigned long long last_ack_down_timestamp;
#endif
};

#ifdef DELAYS_WITH_ERROR
struct Arbiter_history_item_
{
    unsigned long long timestamp;
    float error;
};

void BrzArbiter_AddEventInHistory (struct BrzArbiter_ *c)
{
    struct Arbiter_history_item_ *item = g_new0 (struct Arbiter_history_item_, 1);

    item->timestamp = current_real_time;
    item->error = current_error;
    c->history = g_list_prepend (c->history, item);

    unsigned long long min1, min2, max1, max2;

    min1 = current_real_time - (unsigned long long) (current_error + 0.999999);
    max1 = current_real_time + (unsigned long long) (current_error + 0.999999);

    gboolean non_determinism_detected = FALSE;

    printf ("Arbiter %p history: ", c);
    GList *tmp;

    for (tmp = c->history; tmp; tmp = tmp->next)
    {
        item = tmp->data;

        min2 = item->timestamp - (unsigned long long) (item->error + 0.999999);
        max2 = item->timestamp + (unsigned long long) (item->error + 0.999999);
        printf ("%lld~%f=%lld-%lld ", item->timestamp, item->error, min2, max2);

        if (tmp != c->history && min1 < max2)
        {
            non_determinism_detected = TRUE;
            printf ("!  ");
        }
    }
    printf (".\n");
    if (non_determinism_detected)
        printf ("      non_determinism_detected !!!!!\n");
}
#endif

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzArbiter2_requp (struct comp *comp, int port);
void BrzArbiter_requp (struct comp *comp, int port)
{
    struct BrzArbiter_ *c = (struct BrzArbiter_ *) comp;

    InsertReorderedEventAtCurrentTime (&c->reordered_req_event[port]);
}

void BrzArbiter2_requp (struct comp *comp, int port)
#else //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
void BrzArbiter_requp (struct comp *comp, int port)
#endif                          //#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
{
    struct BrzArbiter_ *c = (struct BrzArbiter_ *) comp;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    current_real_time = MAX (c->last_ack_down_timestamp, current_real_time);
#endif

    switch (port)
    {
    case 0:
        if (c->state == idle1idle2)
        {
            c->state = busy1idle2;
            TRACE_FOLLOW_THREAD (2, 0);
#ifdef DELAYS_WITH_ERROR
            BrzArbiter_AddEventInHistory (c);
#endif
            SEND_REQUP (2);
        } else
        {
            c->state = busy2waiting1;
        }
        break;
    case 1:
        if (c->state == idle1idle2)
        {
            c->state = busy2idle1;
            TRACE_FOLLOW_THREAD (3, 1);
#ifdef DELAYS_WITH_ERROR
            BrzArbiter_AddEventInHistory (c);
#endif
            SEND_REQUP (3);
        } else
        {
            c->state = busy1waiting2;
        }
        break;
    }
}
void BrzArbiter_ackup (struct comp *comp, int port)
{
    struct BrzArbiter_ *c = (struct BrzArbiter_ *) comp;

    SEND_ACKUP (port - 2);
}

void BrzArbiter_reqdown (struct comp *comp, int port)
{
    struct BrzArbiter_ *c = (struct BrzArbiter_ *) comp;

    SEND_REQDOWN (port + 2);
}

void BrzArbiter_ackdown (struct comp *comp, int port)
{
    struct BrzArbiter_ *c = (struct BrzArbiter_ *) comp;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->last_ack_down_timestamp = current_real_time;
#endif

    switch (c->state)
    {
    case busy1idle2:
        c->state = idle1idle2;
        SEND_ACKDOWN (0);
        break;
    case busy1waiting2:
        c->state = busy2idle1;
        TRACE_FOLLOW_THREAD (3, 1);
#ifdef DELAYS_WITH_ERROR
        BrzArbiter_AddEventInHistory (c);
#endif
        SEND_REQUP (3);
        SEND_ACKDOWN (0);
        break;
    case busy2idle1:
        c->state = idle1idle2;
        SEND_ACKDOWN (1);
        break;
    case busy2waiting1:
        c->state = busy1idle2;
        TRACE_FOLLOW_THREAD (2, 0);
#ifdef DELAYS_WITH_ERROR
        BrzArbiter_AddEventInHistory (c);
#endif
        SEND_REQUP (2);
        SEND_ACKDOWN (1);
        break;
    default:
        fprintf (stderr, "Fatal error in BrzArbiter_ackdown\n");
        CloseBreezeSimulation (2);
    }
}

//void BrzArbiter (int num, int in1, int in2, int out1, int out2)
void BrzArbiter (int num, char *args[])
{
    INIT_COMP (BrzArbiter);
    INIT_COMP_PASSIVE_PORT (BrzArbiter, 0, atoi (args[0]));
    INIT_COMP_PASSIVE_PORT (BrzArbiter, 1, atoi (args[1]));
    INIT_COMP_ACTIVE_PORT (BrzArbiter, 2, atoi (args[2]));
    INIT_COMP_ACTIVE_PORT (BrzArbiter, 3, atoi (args[3]));

    c->state = idle1idle2;

#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->reordered_req_event[0].cb = &c->reordered_req_cb[0];
    c->reordered_req_cb[0].fct = BrzArbiter2_requp;
    c->reordered_req_cb[0].comp = (struct comp *) c;
    c->reordered_req_cb[0].portnum = 0;

    c->reordered_req_event[1].cb = &c->reordered_req_cb[1];
    c->reordered_req_cb[1].fct = BrzArbiter2_requp;
    c->reordered_req_cb[1].comp = (struct comp *) c;
    c->reordered_req_cb[1].portnum = 1;
#endif

    WRITE_DEBUG_INFO_COMP ("Arbiter", 4);
}

/****************************************************/

struct BrzCall_
{
    STRUCT_COMP_BASE;

    int nb_inputs;
    int active_port;
};

void BrzCall_requp (struct comp *comp, int port)
{
    struct BrzCall_ *c = (struct BrzCall_ *) comp;

    c->active_port = port;
    TRACE_FOLLOW_THREAD (c->nb_inputs, port);
    SEND_REQUP_DELAYED (c->nb_inputs, 2 * BREEZE_CONTROL_DELAY);
}

void BrzCall_ackup (struct comp *comp, int port)
{
    struct BrzCall_ *c = (struct BrzCall_ *) comp;

    SEND_ACKUP (c->active_port);
}

void BrzCall_reqdown (struct comp *comp, int port)
{
    struct BrzCall_ *c = (struct BrzCall_ *) comp;

    if (port != c->active_port)
    {
        fprintf (stderr, "BrzCall_reqdown: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }

    SEND_REQDOWN_DELAYED (c->nb_inputs, BREEZE_CONTROL_DELAY);
}

void BrzCall_ackdown (struct comp *comp, int port)
{
    struct BrzCall_ *c = (struct BrzCall_ *) comp;

    SEND_ACKDOWN (c->active_port);
}

//void BrzCall (int num, int nb_inputs, ...) //int in1, int in2, ..., int out
void BrzCall (int num, char *args[])
{
    int nb_inputs = atoi (args[0]);
    int i;

    INIT_COMP (BrzCall);

    c->nb_inputs = nb_inputs;

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzCall, i, atoi (args[i + 1]));
    }

    INIT_COMP_ACTIVE_PORT (BrzCall, nb_inputs, atoi (args[nb_inputs + 1]));

    WRITE_DEBUG_INFO_COMP ("Call", c->nb_inputs + 1);
}

/****************************************************/

struct BrzDecisionWait_
{
    STRUCT_COMP_BASE;

    int nb_decisions;
    int state;                  // -1=no activation&no input ; 0=activation&no input ; -x-1=no activation&input x ; x=activation&input x
    unsigned long long max_event_time;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    unsigned long long last_processed_event_time;
#endif
};

void BrzDecisionWait_requp (struct comp *comp, int port)
{
    struct BrzDecisionWait_ *c = (struct BrzDecisionWait_ *) comp;

    if (c->state == -1)
    {
        if (port == 0)
        {
            c->state = 0;
            c->max_event_time = current_real_time;
        } else
        {
            c->state = -1 - port;
            c->max_event_time = current_real_time;
        }
        NO_OPERATION ();
    } else if (c->state == 0)
    {
        if (port != 0)
        {
            c->state = port;
        }
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        if (c->last_processed_event_time > current_real_time || c->last_processed_event_time > c->max_event_time)
        {
            fprintf (stderr,
              "*************************************\n"
              "Race condition detected in a DecisionWait component:\n"
              " - Previous event processed at time %lld\n"
              " - current event 1 at time %lld\n"
              " - current event 2 at time %lld\n"
              "*************************************\n", c->last_processed_event_time, c->max_event_time, current_real_time);
            c->last_processed_event_time = MAX (c->max_event_time, current_real_time);
            NO_OPERATION ();
            return;
        }
        c->last_processed_event_time = MAX (c->max_event_time, current_real_time);
#endif

        current_real_time = MAX (c->max_event_time, current_real_time);
        TRACE_FOLLOW_THREAD (c->state + c->nb_decisions, c->state);
        SEND_REQUP_DELAYED (c->state + c->nb_decisions, 3 * BREEZE_CONTROL_DELAY);
    } else
    {
        if (port != 0)
        {
            fprintf (stderr,
              "Fatal Error: You have an error in your Balsa program: 2 \"select\" cases are occuring simultaneously. (Error detected by the DecisionWait component %d)\n",
              c->comp_num);
            fprintf (stderr, "Corresponding channels: %ld and %ld\n", GET_CHAN_NUM (c->chan[port]),
              GET_CHAN_NUM (c->chan[(c->state > 0) ? c->state : (-1 - c->state)]));
            CloseBreezeSimulation (2);
        } else
        {
            c->state = -1 - c->state;
            current_real_time = MAX (c->max_event_time, current_real_time);
            TRACE_FOLLOW_THREAD (c->state + c->nb_decisions, c->state);
            SEND_REQUP_DELAYED (c->state + c->nb_decisions, 3 * BREEZE_CONTROL_DELAY);
        }
    }
}
void BrzDecisionWait_ackup (struct comp *comp, int port)
{
    struct BrzDecisionWait_ *c = (struct BrzDecisionWait_ *) comp;

    if (port - c->nb_decisions != c->state)
    {
        fprintf (stderr, "BrzDecisionWait_ack: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }
    SEND_ACKUP (port - c->nb_decisions);
    SEND_ACKUP_DELAYED (0, BREEZE_CONTROL_DELAY);
    c->state = -1;
    c->max_event_time = 0;
}

void BrzDecisionWait_reqdown (struct comp *comp, int port)
{
    struct BrzDecisionWait_ *c = (struct BrzDecisionWait_ *) comp;

    if (c->state == -1)
    {
        if (port == 0)
        {
            c->state = 0;
            c->max_event_time = current_real_time;
        } else
        {
            c->state = port;
            c->max_event_time = current_real_time;
        }
        NO_OPERATION ();
    } else if (c->state >= 0)   // normal case (see next else statement)
    {
        if (port != 0)
        {
            c->state = port;
            if (c->max_event_time < current_real_time)
            {
                SEND_REQDOWN_DELAYED (c->state + c->nb_decisions, BREEZE_CONTROL_DELAY);
            } else
            {
                current_real_time = c->max_event_time;
                SEND_REQDOWN_DELAYED (c->state + c->nb_decisions, 2 * BREEZE_CONTROL_DELAY);
            }
        } else
        {
            if (c->max_event_time <= current_real_time)
            {
                SEND_REQDOWN_DELAYED (c->state + c->nb_decisions, 2 * BREEZE_CONTROL_DELAY);
            } else
            {
                current_real_time = c->max_event_time;
                SEND_REQDOWN_DELAYED (c->state + c->nb_decisions, BREEZE_CONTROL_DELAY);
            }
        }
    } else                      // c->state < -1: the reqdown is happening before the DW got activated. This happens when a CallDemuxPush components sends the RTZ to a useless branch.
    {
        if (port == -1 - c->state)
        {
            current_real_time = MAX (c->max_event_time, current_real_time);
            SEND_ACKDOWN (port);
            c->state = -1;
            c->max_event_time = 0;
        } else
        {
            fprintf (stderr, "*************************************\n" "Strange thing detected in a DecisionWait component:\n"
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
              " - Previous event processed at time %lld\n"
#endif
              " - current event 1 at time %lld\n" " - current event 2 at time %lld\n" "*************************************\n",
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
              c->last_processed_event_time,
#endif
              c->max_event_time, current_real_time);
        }
    }
}
void BrzDecisionWait_ackdown (struct comp *comp, int port)
{
    struct BrzDecisionWait_ *c = (struct BrzDecisionWait_ *) comp;

    if (port - c->nb_decisions != c->state)
    {
        fprintf (stderr, "BrzDecisionWait_ack: ASSERT failed\n");
        CloseBreezeSimulation (2);
    }
    SEND_ACKDOWN (port - c->nb_decisions);
    SEND_ACKDOWN_DELAYED (0, BREEZE_CONTROL_DELAY);

    c->state = -1;
    c->max_event_time = 0;
}

//void BrzDecisionWait (int num, int nb_decisions, int in0, ...)
void BrzDecisionWait (int num, char *args[])
{
    int nb_decisions = atoi (args[0]);
    int i;

    INIT_COMP (BrzDecisionWait);
    INIT_COMP_PASSIVE_PORT (BrzDecisionWait, 0, atoi (args[1]));

    c->nb_decisions = nb_decisions;
    c->state = -1;
    c->max_event_time = 0;

    for (i = 0; i < nb_decisions; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzDecisionWait, i + 1, atoi (args[i + 2]));
    }

    for (i = 0; i < nb_decisions; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzDecisionWait, i + nb_decisions + 1, atoi (args[i + 2 + nb_decisions]));
    }

    WRITE_DEBUG_INFO_COMP ("DW", 1 + 2 * c->nb_decisions);
}

/****************************************************/

struct BrzSequence_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_T_elements;
    int nb_acks;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    unsigned long long max_event_time;
#endif
};

void BrzSequence_requp (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP_DELAYED (1, 3 * BREEZE_CONTROL_DELAY);
    c->nb_acks = 0;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->max_event_time = 0;
#endif
}

void BrzSequence_ackup (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    if (port != c->nb_outs)
    {
        SEND_REQDOWN (port);
    } else
    {
        SEND_ACKUP (0);
    }
}

void BrzSequence_ackup_optimised (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    if (port != c->nb_outs)
    {
        SEND_REQUP (port + 1);
        SEND_REQDOWN (port);
    } else
    {
        SEND_ACKUP (0);
    }
}

void BrzSequence_reqdown (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    c->nb_acks++;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
    c->max_event_time = MAX (c->max_event_time, current_real_time);
#endif
    if (c->nb_acks == c->nb_T_elements + 1)
    {
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        current_real_time = c->max_event_time;
#endif
        SEND_REQDOWN (c->nb_outs);
    }
}

void BrzSequence_ackdown (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    if (port == c->nb_outs)
    {
        SEND_ACKDOWN (0);
    } else
    {
        TRACE_FOLLOW_THREAD (port + 1, port);
        SEND_REQUP_DELAYED (port + 1, BREEZE_CONTROL_DELAY);
    }
}

void BrzSequence_ackdown_optimised (struct comp *comp, int port)
{
    struct BrzSequence_ *c = (struct BrzSequence_ *) comp;

    if (port == c->nb_outs)
    {
        SEND_ACKDOWN (0);
    } else
    {
        c->nb_acks++;
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
        c->max_event_time = MAX (c->max_event_time, current_real_time);
#endif
        if (c->nb_acks == c->nb_T_elements + 1)
        {
#ifdef REORDERED_EVENT_DRIVEN_SCHEDULER
            current_real_time = c->max_event_time;
#endif
            //      TRACE_FOLLOW_THREAD (port + 1, port);
            SEND_REQDOWN_DELAYED (c->nb_outs, BREEZE_CONTROL_DELAY);
        }
    }
}

//void BrzSequence (int num, int nb_outs, int in, ...)
void BrzSequence (int num, char *args[])
{
    int nb_outs = atoi (args[0]);
    char *specString = args[1];
    int i;

    g_assert (specString[0] == '\"');

    INIT_COMP (BrzSequence);
    INIT_COMP_PASSIVE_PORT (BrzSequence, 0, atoi (args[2]));

    c->nb_outs = nb_outs;
    c->nb_T_elements = 0;

    for (i = 0; i < nb_outs; i++)
    {
        int channum = atoi (args[i + 3]);

        INIT_COMP_ACTIVE_PORT (BrzSequence, i + 1, channum);
    }

    for (i = 0; i < nb_outs - 1; i++)
    {
        switch (specString[i + 1])
        {
        case 'S':
            break;
        case 'T':
            {
                int channum = atoi (args[i + 3]);

                channel[channum].ackup.fct = BrzSequence_ackup_optimised;
                channel[channum].ackdown.fct = BrzSequence_ackdown_optimised;
                c->nb_T_elements++;
            }
            break;
        default:
            fprintf (stderr, "Error in BrzSequence's specString (%s), position %d\n", specString, i + 1);
            exit (EXIT_FAILURE);
        }
    }

    WRITE_DEBUG_INFO_COMP (";", 1 + nb_outs);
}

/****************************************************/

struct BrzConcur_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_acknowledges;
    unsigned long long max_event_time;
};

void BrzConcur_requp (struct comp *comp, int port)
{
    int i;
    struct BrzConcur_ *c = (struct BrzConcur_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        TRACE_NEW_THREAD (i + 1, 0);
        SEND_REQUP_DELAYED (i + 1, 0 /*3 * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzConcur_ackup (struct comp *comp, int port)
{
    struct BrzConcur_ *c = (struct BrzConcur_ *) comp;

    SEND_REQDOWN (port);
}

void BrzConcur_reqdown (struct comp *comp, int port)
{
    struct BrzConcur_ *c = (struct BrzConcur_ *) comp;

    SEND_ACKDOWN (0);
}

void BrzConcur_ackdown (struct comp *comp, int port)
{
    struct BrzConcur_ *c = (struct BrzConcur_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}

//void BrzConcur (int num, int nb_outs, int in, ...)
void BrzConcur (int num, char *args[])
{
    int nb_outs = atoi (args[0]);
    int i;

    INIT_COMP (BrzConcur);
    INIT_COMP_PASSIVE_PORT (BrzConcur, 0, atoi (args[1]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzConcur, i + 1, atoi (args[i + 2]));
    }

    WRITE_DEBUG_INFO_COMP ("||", 1 + c->nb_outs);
}

/****************************************************/

/****************************************************/

struct BrzBuiltinVariable_
{
    STRUCT_COMP_BASE;

    int nb_read_ports;
    int width;
    BalsaObject *object;
    mpz_t data;
    char *variableName;
};

void BrzBuiltinVariable_requp (struct comp *comp, int port)
{
    struct BrzBuiltinVariable_ *c = (struct BrzBuiltinVariable_ *) comp;

    if (port == 0)
    {
        BalsaObject *newObject = BALSA_OBJECT (GetPointerFromMpzT ((mpz_ptr) GET_PORT_DATA (0)));

        BalsaObjectRef (newObject);
        BalsaObjectUnref (c->object);
        c->object = newObject;
        SetMpzTFromPointer (c->data, c->object);

        if (!c->object->data)
        {
            fprintf (stderr, "builtin variable assigned with uninitialised value `%s'\n", c->variableName);
        }

        SEND_ACKUP (0);
    } else
    {
        if (!c->object->data)
        {
            fprintf (stderr, "Uninitialised builtin variable read `%s'\n", c->variableName);
        }

        SET_PORT_DATA (port, c->data);
        SEND_DATAON_DELAYED (port, 0);
        SEND_ACKUP_DELAYED (port, 1);
    }
}
void BrzBuiltinVariable_ackup (struct comp *comp, int port)
{
    //    struct BrzBuiltinVariable_ *c = (struct BrzBuiltinVariable_ *) comp;
}
void BrzBuiltinVariable_reqdown (struct comp *comp, int port)
{
    struct BrzBuiltinVariable_ *c = (struct BrzBuiltinVariable_ *) comp;

    SEND_ACKDOWN_DELAYED (port, 0);

    if (port >= 1)
    {
        SEND_DATAOFF_DELAYED (port, 1);
    }
}

void BrzBuiltinVariable_ackdown (struct comp *comp, int port)
{
    //    struct BrzBuiltinVariable_ *c = (struct BrzBuiltinVariable_ *) comp;
}

//void BrzBuiltinVariable (int num, int width, int nb_read_ports, char *name, ...)
void BrzBuiltinVariable (int num, char *args[])
{
    int nb_read_ports = atoi (args[0]);
    char *name = (char *) args[1];
    int i;

    INIT_COMP (BrzBuiltinVariable);
    INIT_COMP_PASSIVE_PORT (BrzBuiltinVariable, 0, atoi (args[2]));

    c->object = NewBalsaObject (NULL, NULL);
    BalsaObjectRef (c->object);
    mpz_init (c->data);
    SetMpzTFromPointer (c->data, c->object);

    c->nb_read_ports = nb_read_ports;
    c->variableName = g_strdup (name + 1); /* Skip leading " */

    for (i = 1; i <= nb_read_ports; i++)
    {
        int chan_num = atoi (args[i + 2]);

        INIT_COMP_PASSIVE_PORT (BrzBuiltinVariable, i, chan_num);
        INIT_COMP_CONTROLLED_DATA_PORT (i, chan_num);
    }

    WRITE_DEBUG_INFO_COMP (name, 1 + c->nb_read_ports);
}

/****************************************************/

struct BrzConstant_
{
    STRUCT_COMP_BASE;
};

void BrzConstant_requp (struct comp *comp, int port)
{
    struct BrzConstant_ *c = (struct BrzConstant_ *) comp;

    FAKE_SET_PORT_DATA_FOR_DEBUG (0);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, 1);
}

void BrzConstant_ackup (struct comp *comp, int port)
{
    //    struct BrzConstant_ *c = (struct BrzConstant_ *) comp;
}
void BrzConstant_reqdown (struct comp *comp, int port)
{
    struct BrzConstant_ *c = (struct BrzConstant_ *) comp;

    SEND_ACKDOWN_DELAYED (0, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

void BrzConstant_ackdown (struct comp *comp, int port)
{
    //    struct BrzConstant_ *c = (struct BrzConstant_ *) comp;
}

//void BrzConstant (int num, int width, int value, int in)
void BrzConstant (int num, char *args[])
{
    int width = atoi (args[0]);

    INIT_COMP (BrzConstant);
    INIT_COMP_PASSIVE_PORT (BrzConstant, 0, atoi (args[2]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[2]));

    if (width <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        //      sscanf (((char *) args[1]), "%u", &(GET_PORT_DATA (0)));
        GET_PORT_DATA (0) = (long) atoll ((char *) args[1]);
    } else
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
        mpz_set_str ((mpz_ptr) GET_PORT_DATA (0), (char *) args[1], 10);
    }

    {
        char *desc = g_strdup_printf ("Value%s", (char *) args[1]);

        WRITE_DEBUG_INFO_COMP (desc, 1);
        free (desc);
    }
}

/****************************************************/

struct BrzFetch_
{
    STRUCT_COMP_BASE;
};

void BrzFetch_requp (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

//Default implementation is reduced broad: void BrzFetch_ackup_reduced_broad (struct comp *comp, int port)
void BrzFetch_ackup (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    switch (port)
    {
    case 1:
        COPY_DATA_FROM_TO (1, 2);
        TRACE_FOLLOW_THREAD (2, 0);
        SEND_DATAON_DELAYED (2, 0);
        SEND_REQUP_DELAYED (2, 1);
        break;
    case 2:
        SEND_ACKUP (0);
        break;
    }
}
void BrzFetch_ackup_broad (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    switch (port)
    {
    case 1:
        COPY_DATA_FROM_TO (1, 2);
        TRACE_FOLLOW_THREAD (2, 0);
        SEND_DATAON_DELAYED (2, 0);
        SEND_REQUP_DELAYED (2, 1);
        break;
    case 2:
        SEND_REQDOWN (2);
        break;
    }
}

void BrzFetch_reqdown (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    SEND_REQDOWN (1);
}

//Default implementation is reduced broad: void BrzFetch_ackdown_reduced_broad (struct comp *comp, int port)
void BrzFetch_ackdown (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    switch (port)
    {
    case 1:
        SEND_REQDOWN_DELAYED (2, 0);
        SEND_DATAOFF_DELAYED (2, 1);
        break;
    case 2:
        SEND_ACKDOWN_DELAYED (0, 0);
        break;
    }
}
void BrzFetch_ackdown_broad (struct comp *comp, int port)
{
    struct BrzFetch_ *c = (struct BrzFetch_ *) comp;

    switch (port)
    {
    case 1:
        SEND_ACKDOWN_DELAYED (0, 0);
        break;
    case 2:
        SEND_ACKUP (0);
        SEND_DATAOFF_DELAYED (2, 1);
        break;
    }
}

//void BrzFetch (int num, int width, bool outBroad, int in, int out1, int out2)
void BrzFetch (int num, char *args[])
{
    int isOutBroad = arg2bool (args[1]);
    int in = atoi (args[2]);
    int out1 = atoi (args[3]);
    int out2 = atoi (args[4]);

    INIT_COMP (BrzFetch);
    INIT_COMP_PASSIVE_PORT (BrzFetch, 0, in);
    INIT_COMP_ACTIVE_PORT (BrzFetch, 1, out1);
    INIT_COMP_ACTIVE_PORT (BrzFetch, 2, out2);
    INIT_COMP_CONTROLLED_DATA_PORT (2, out2);

    if (isOutBroad)
    {
        channel[out1].ackup.fct = BrzFetch_ackup_broad;
        channel[out1].ackdown.fct = BrzFetch_ackdown_broad;
        channel[out2].ackup.fct = BrzFetch_ackup_broad;
        channel[out2].ackdown.fct = BrzFetch_ackdown_broad;
    }

    WRITE_DEBUG_INFO_COMP ("->", 3);
}

/****************************************************/

struct BrzAdapt_
{
    STRUCT_COMP_BASE;

    int width_in;
    int width_out;
    int inIsSigned;
    int outIsSigned;

    mpz_t mpint_mask_in;
    mpz_t mpint_mask_out;
};

void BrzAdapt_requp (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP_DELAYED (1, BREEZE_DATAPATH_DELAY);
}

void BrzAdapt_ackup (struct comp *comp, int port)
{
    printf ("BrzAdapt_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzAdapt_ackup_int_int (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    int data = GET_PORT_DATA (1);

    if (c->width_out > c->width_in)
    {
        data = data & ((1 << c->width_in) - 1);
    } else
    {
        if (c->inIsSigned /* outIsSigned is deprecated AB 2008-03-10 */)
        {
            if (data & (1 << (c->width_out - 1)))
                data = (data | ~((1 << c->width_out) - 1));

            if (c->width_in != (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                data = data & ((1 << c->width_in) - 1);
        }
    }
    SET_PORT_DATA (0, data);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, 1);
}

void BrzAdapt_ackup_mpint_int (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    int data = mpz_get_ui ((mpz_ptr) GET_PORT_DATA (1));

    if (c->width_in != (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        data = data & ((1 << c->width_in) - 1);
    SET_PORT_DATA (0, data);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, 1);
}

void BrzAdapt_ackup_mpint_mpint (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    mpz_set ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (1));
    if (c->width_out > c->width_in)
    {
        mpz_and ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (1), c->mpint_mask_in); //mask_in=((1<<width_in)-1)
    } else
    {
        if (c->inIsSigned /* outIsSigned is deprecated AB 2008-03-10 */)
        {
            if (mpz_tstbit ((mpz_ptr) GET_PORT_DATA (0), c->width_out - 1))
                mpz_ior ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (1), c->mpint_mask_out); //mask_out= ~((1<<width_out)-1)
        }
    }
    FAKE_SET_PORT_DATA_FOR_DEBUG (0);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, 1);

    /*
       GET_DATA_FROM_TO(out,1);
       if (c->width_out > c->width_in) {
       mpz_and ((mpz_ptr)c->data[0], (mpz_ptr)c->data[1], c->mpint_mask_in); //mask_in=((1<<width_in)-1)
       } else {
       if (c->inIsSigned) {
       if (mpz_tstbit ((mpz_ptr)c->data[0], c->width_out-1))
       mpz_ior ((mpz_ptr)c->data[0], (mpz_ptr)c->data[1], c->mpint_mask_out); //mask_out= ~((1<<width_out)-1)
       }
       }
       FAKE_SET_PORT_DATA_FOR_DEBUG (0);
       SEND_ACKUP(0);
     */
}

void BrzAdapt_ackup_int_mpint (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    mpz_set_ui ((mpz_ptr) GET_PORT_DATA (0), GET_PORT_DATA (1));

    if (c->inIsSigned /* outIsSigned is deprecated AB 2008-03-10 */)
    {
        if (GET_PORT_DATA(1) & (1 << (c->width_out - 1)))
            mpz_ior ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (0), c->mpint_mask_out); //mask_out= ~((1<<width_out)-1)
    }

    FAKE_SET_PORT_DATA_FOR_DEBUG (0);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, 1);

    /*
       GET_DATA_FROM_TO(out,1);
       mpz_set ((mpz_ptr)c->data[0], (mpz_ptr)c->data[1]);
       if (c->width_out > c->width_in) {
       mpz_and ((mpz_ptr)c->data[0], (mpz_ptr)c->data[0], c->mpint_mask_in); //mask_in=((1<<width_in)-1)
       } else {
       if (c->inIsSigned) {
       if (mpz_tstbit ((mpz_ptr)c->data[0], c->width_out-1))
       mpz_ior ((mpz_ptr)c->data[0], (mpz_ptr)c->data[0], c->mpint_mask_out); //mask_out= ~((1<<width_out)-1) over width_in bits
       }
       }
       SEND_ACKUP(0);
     */
}

void BrzAdapt_reqdown (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    SEND_REQDOWN_DELAYED (1, BREEZE_CONTROL_DELAY);
}

void BrzAdapt_ackdown (struct comp *comp, int port)
{
    struct BrzAdapt_ *c = (struct BrzAdapt_ *) comp;

    SEND_ACKDOWN_DELAYED (0, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzAdapt (int num, int width_in, int width_out, int inIsSigned, int outIsSigned, int in, int out)
void BrzAdapt (int num, char *args[])
{
    int width_in = atoi (args[0]);
    int width_out = atoi (args[1]);
    int outIsSigned = arg2bool (args[2]);
    int inIsSigned = arg2bool (args[3]);
    int out = atoi (args[5]);

    INIT_COMP (BrzAdapt);
    INIT_COMP_PASSIVE_PORT (BrzAdapt, 0, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzAdapt, 1, out);
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[4]));

    if ((width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzAdapt_ackup_mpint_mpint;
    else if ((width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzAdapt_ackup_mpint_int;
    else if ((width_out <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzAdapt_ackup_int_mpint;
    else                        //if ((width_out<=(sizeof(int)*8)) && (width_in<=(sizeof(int)*8)))
        channel[out].ackup.fct = BrzAdapt_ackup_int_int;

    if (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));

        //mask_in=((1<<width_in)-1)
        mpz_init (c->mpint_mask_in);
        mpz_setbit (c->mpint_mask_in, width_in);
        mpz_sub_ui (c->mpint_mask_in, c->mpint_mask_in, 1);

        //mask_out= ~((1<<width_out)-1) over width_in bits
        mpz_init_set (c->mpint_mask_out, c->mpint_mask_in);
        mpz_clrbit (c->mpint_mask_out, width_out);
        mpz_add_ui (c->mpint_mask_out, c->mpint_mask_out, 1);
    }

    if (width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
    }

    c->width_in = width_in;
    c->width_out = width_out;
    c->inIsSigned = inIsSigned;
    c->outIsSigned = outIsSigned;

    WRITE_DEBUG_INFO_COMP ("Adapt", 2);
}

/****************************************************/

struct BrzSlice_
{
    STRUCT_COMP_BASE;

    int width_in;
    int width_out;
    int lowIndex;

    mpz_t mpint_tmp;
};

void BrzSlice_requp (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

void BrzSlice_ackup (struct comp *comp, int port)
{
    printf ("BrzSlice_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzSlice_ackup_int_int (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    int data = GET_PORT_DATA (1);

    data = (data >> c->lowIndex) & ((1 << c->width_in) - 1);
    SET_PORT_DATA (0, data);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzSlice_ackup_mpint_int (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    mpz_tdiv_q_2exp (c->mpint_tmp, (mpz_ptr) GET_PORT_DATA (1), c->lowIndex);
    mpz_tdiv_r_2exp (c->mpint_tmp, c->mpint_tmp, c->width_in);
    SET_PORT_DATA (0, mpz_get_ui (c->mpint_tmp));
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzSlice_ackup_mpint_mpint (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    mpz_tdiv_q_2exp (c->mpint_tmp, (mpz_ptr) GET_PORT_DATA (1), c->lowIndex);
    mpz_tdiv_r_2exp (c->mpint_tmp, c->mpint_tmp, c->width_in);
    SET_PORT_DATA (0, (long) c->mpint_tmp);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzSlice_reqdown (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    SEND_REQDOWN_DELAYED (1, BREEZE_CONTROL_DELAY);
}

void BrzSlice_ackdown (struct comp *comp, int port)
{
    struct BrzSlice_ *c = (struct BrzSlice_ *) comp;

    SEND_ACKDOWN_DELAYED (0, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzSlice (int num, int width_in, int width_out, int lowIndex, int in, int out)
void BrzSlice (int num, char *args[])
{
    int width_in = atoi (args[0]);
    int width_out = atoi (args[1]);
    int lowIndex = atoi (args[2]);
    int out = atoi (args[4]);

    INIT_COMP (BrzSlice);
    INIT_COMP_PASSIVE_PORT (BrzSlice, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzSlice, 1, out);
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));

    if ((width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzSlice_ackup_mpint_mpint;
    else if ((width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzSlice_ackup_mpint_int;
    else if ((width_out <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)) && (width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)))
        channel[out].ackup.fct = BrzSlice_ackup_int_int;
    else                        //if ((width_out<=(sizeof(int)*8)) && (width_in>(sizeof(int)*8)))
        //  channel[out].ackup.fct = BrzSlice_ackup_int_mpint;
    {
        fprintf (stderr, "BrzSlice error: ((width_out<=(sizeof(int)*8)) && (width_in>(sizeof(int)*8)))\n");
        CloseBreezeSimulation (2);
    }

    if (width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        mpz_init (c->mpint_tmp);
    }

    c->width_in = width_in;
    c->width_out = width_out;
    c->lowIndex = lowIndex;

    WRITE_DEBUG_INFO_COMP ("Slice", 2);
}

/****************************************************/

struct BrzUnaryFunc_
{
    STRUCT_COMP_BASE;

    enum UnaryFuncType type;
    int width_res;
    int width_arg;
    int isSigned;

    mpz_t mpint_tmp1, mpint_tmp2, mpint_mask_res;
};

void BrzUnaryFunc_Process_int (struct BrzUnaryFunc_ *c)
{
    int arg = GET_PORT_DATA (1);
    int result;

    if (c->isSigned)
        arg = EXPAND_INT_SIGN (arg, c->width_arg);

    switch (c->type)
    {
    case Invert:
        result = ~arg;
        break;
    case Negate:
        result = -arg;
        break;
    }

    if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        result &= (1 << c->width_res) - 1;

    SET_PORT_DATA (0, result);
}

void BrzUnaryFunc_Process_mpint (struct BrzUnaryFunc_ *c)
{
    mpz_ptr arg;
    int result;

    if (c->width_arg > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        arg = (mpz_ptr) GET_PORT_DATA (1);
    else
    {
        if (c->isSigned)
        {
            int tmp = GET_PORT_DATA (1);

            tmp = EXPAND_INT_SIGN (tmp, c->width_arg);
            mpz_set_si (c->mpint_tmp1, tmp);
        } else
            mpz_set_ui (c->mpint_tmp1, GET_PORT_DATA (1));
        arg = c->mpint_tmp1;
    }

    switch (c->type)
    {
    case Invert:
        mpz_com (c->mpint_tmp2, arg);
        break;
    case Negate:
        mpz_neg (c->mpint_tmp2, arg);
        break;
    }

    mpz_and (c->mpint_tmp2, c->mpint_tmp2, c->mpint_mask_res); //mask_res=((1<<width_res)-1)

    if (c->width_res > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        result = (long) c->mpint_tmp2;
    } else
    {
        if (c->isSigned)
            result = mpz_get_si (c->mpint_tmp2);
        else
            result = mpz_get_ui (c->mpint_tmp2);

        if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            result &= (1 << c->width_res) - 1;
    }

    SET_PORT_DATA (0, result);
}

void BrzUnaryFunc_requp (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFunc_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

void BrzUnaryFunc_ackup (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFunc_ *) comp;

    BrzUnaryFunc_Process_int (c);

    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzUnaryFunc_ackup_mpint (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFunc_ *) comp;

    BrzUnaryFunc_Process_mpint (c);

    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzUnaryFunc_reqdown (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFunc_ *) comp;

    SEND_REQDOWN (1);
}

void BrzUnaryFunc_ackdown (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFunc_ *) comp;

    SEND_ACKDOWN_DELAYED (0, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzUnaryFunc (int num, int width_res, int width_arg, enum UnaryFuncType type, int isSigned_arg, int in, int out)
void BrzUnaryFunc (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg = atoi (args[1]);
    enum UnaryFuncType type = arg2UnaryFuncType (args[2]);
    int isSigned = arg2bool (args[3]);
    int in = atoi (args[4]);
    int out = atoi (args[5]);

    INIT_COMP (BrzUnaryFunc);
    INIT_COMP_PASSIVE_PORT (BrzUnaryFunc, 0, in);
    INIT_COMP_ACTIVE_PORT (BrzUnaryFunc, 1, out);
    INIT_COMP_CONTROLLED_DATA_PORT (0, in);

    if (width_arg > (HOST_INTEGER_WIDTH_IN_BYTES * 8) || width_res > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        channel[out].ackup.fct = BrzUnaryFunc_ackup_mpint;

        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }
    c->width_res = width_res;
    c->width_arg = width_arg;
    c->type = type;
    c->isSigned = isSigned;

    switch (c->type)
    {
    case Invert:
        WRITE_DEBUG_INFO_COMP ("~_", 2);
        break;
    case Negate:
        WRITE_DEBUG_INFO_COMP ("-_", 2);
        break;
    default:
        WRITE_DEBUG_INFO_COMP ("UnaryFunc", 2);
    }

}

/****************************************************/

#define BrzUnaryFuncPush_ BrzUnaryFunc_

void BrzUnaryFuncPush_requp (struct comp *comp, int port)
{
    struct BrzUnaryFunc_ *c = (struct BrzUnaryFuncPush_ *) comp;

    BrzUnaryFunc_Process_int (c);

    TRACE_FOLLOW_THREAD (0, 1);

    SEND_DATAON_DELAYED (0, 0);
    SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzUnaryFuncPush_requp_mpint (struct comp *comp, int port)
{
    struct BrzUnaryFuncPush_ *c = (struct BrzUnaryFuncPush_ *) comp;

    BrzUnaryFunc_Process_mpint (c);

    TRACE_FOLLOW_THREAD (0, 1);

    SEND_DATAON_DELAYED (0, 0);
    SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzUnaryFuncPush_ackup (struct comp *comp, int port)
{
    struct BrzUnaryFuncPush_ *c = (struct BrzUnaryFuncPush_ *) comp;

    SEND_ACKUP (1);
}

void BrzUnaryFuncPush_reqdown (struct comp *comp, int port)
{
    struct BrzUnaryFuncPush_ *c = (struct BrzUnaryFuncPush_ *) comp;

    SEND_REQDOWN (0);
}

void BrzUnaryFuncPush_ackdown (struct comp *comp, int port)
{
    struct BrzUnaryFuncPush_ *c = (struct BrzUnaryFuncPush_ *) comp;

    SEND_ACKDOWN_DELAYED (1, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzUnaryFuncPush (int num, int width_res, int width_arg, enum UnaryFuncPushType type, int isSigned_arg, int in, int out)
void BrzUnaryFuncPush (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg = atoi (args[1]);
    enum UnaryFuncType type = arg2UnaryFuncType (args[2]);
    int isSigned = arg2bool (args[3]);
    int in = atoi (args[4]);
    int out = atoi (args[5]);

    INIT_COMP (BrzUnaryFuncPush);
    INIT_COMP_ACTIVE_PORT (BrzUnaryFuncPush, 0, in);
    INIT_COMP_PASSIVE_PORT (BrzUnaryFuncPush, 1, out);

    if (width_arg > (HOST_INTEGER_WIDTH_IN_BYTES * 8) || width_res > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        channel[out].requp.fct = BrzUnaryFuncPush_requp_mpint;

        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }
    c->width_res = width_res;
    c->width_arg = width_arg;
    c->type = type;
    c->isSigned = isSigned;

    switch (c->type)
    {
    case Invert:
        WRITE_DEBUG_INFO_COMP ("~_", 2);
        break;
    case Negate:
        WRITE_DEBUG_INFO_COMP ("-_", 2);
        break;
    default:
        WRITE_DEBUG_INFO_COMP ("UnaryFuncPush", 2);
    }

}

/****************************************************/

struct BrzCombine_
{
    STRUCT_COMP_BASE;

    int width_out;
    int width_in1;
    int width_in2;

    int in1_active;
    int in2_active;
    unsigned long long max_event_time;

    mpz_t mpint_tmp;
};

void BrzCombine_requp (struct comp *comp, int port)
{
    struct BrzCombine_ *c = (struct BrzCombine_ *) comp;

    TRACE_NEW_THREAD (1, 0);
    SEND_REQUP_DELAYED (1, BREEZE_CONTROL_DELAY);
    TRACE_NEW_THREAD (2, 0);
    SEND_REQUP_DELAYED (2, BREEZE_CONTROL_DELAY);
    c->in1_active = c->in2_active = 0;
}

void BrzCombine_ackup (struct comp *comp, int port)
{
    struct BrzCombine_ *c = (struct BrzCombine_ *) comp;

    switch (port)
    {
    case 1:
        c->in1_active = 1;
        break;
    case 2:
        c->in2_active = 1;
        break;
    }

    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->in1_active && c->in2_active)
    {
        c->in1_active = 0;
        c->in2_active = 0;
        current_real_time = c->max_event_time;
        c->max_event_time = 0;

        if (c->width_out <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            SET_PORT_DATA (0, (GET_PORT_DATA (2) << c->width_in1) | GET_PORT_DATA (1));
        } else
        {
            if (c->width_in2 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                mpz_set_ui ((mpz_ptr) GET_PORT_DATA (0), GET_PORT_DATA (2));
            else
                mpz_set ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (2));

            mpz_mul_2exp ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (0), c->width_in1);

            if (c->width_in1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                mpz_set_ui (c->mpint_tmp, GET_PORT_DATA (1));
            else
                mpz_set (c->mpint_tmp, (mpz_ptr) GET_PORT_DATA (1));

            mpz_ior ((mpz_ptr) GET_PORT_DATA (0), (mpz_ptr) GET_PORT_DATA (0), c->mpint_tmp);

            FAKE_SET_PORT_DATA_FOR_DEBUG_MPINT (0);
        }

        SEND_DATAON_DELAYED (0, 0);
        SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY + BREEZE_CONTROL_DELAY);
    }
}
void BrzCombine_reqdown (struct comp *comp, int port)
{
    struct BrzCombine_ *c = (struct BrzCombine_ *) comp;

    SEND_REQDOWN_DELAYED (1, BREEZE_CONTROL_DELAY);
    SEND_REQDOWN_DELAYED (2, BREEZE_CONTROL_DELAY);
    c->in1_active = c->in2_active = 0;
    c->max_event_time = 0;
}

void BrzCombine_ackdown (struct comp *comp, int port)
{
    struct BrzCombine_ *c = (struct BrzCombine_ *) comp;

    switch (port)
    {
    case 1:
        c->in1_active = 1;
        break;
    case 2:
        c->in2_active = 1;
        break;
    }
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->in1_active && c->in2_active)
    {
        c->in1_active = 0;
        c->in2_active = 0;
        current_real_time = c->max_event_time;
        c->max_event_time = 0;
        SEND_ACKDOWN_DELAYED (0, BREEZE_CONTROL_DELAY);
        SEND_DATAOFF_DELAYED (0, BREEZE_CONTROL_DELAY + 1);
    }
}

//void BrzCombine (int num, int width_out, int width_in1, int width_in2, int out, int in1, int in2)
void BrzCombine (int num, char *args[])
{
    int width_out = atoi (args[0]);
    int width_in1 = atoi (args[1]);
    int width_in2 = atoi (args[2]);

    INIT_COMP (BrzCombine);
    INIT_COMP_PASSIVE_PORT (BrzCombine, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzCombine, 1, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzCombine, 2, atoi (args[5]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));

    if (width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (0) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (0));
        mpz_init (c->mpint_tmp);
    }

    if (width_in1 + width_in2 != width_out)
    {
        fprintf (stderr, "BrzCombine: width_out1+width_out2 != width_in");
        CloseBreezeSimulation (2);
    }
    c->width_out = width_out;
    c->width_in1 = width_in1;
    c->width_in2 = width_in2;

    WRITE_DEBUG_INFO_COMP ("Combine", 3);
}

/****************************************************/

struct BrzBinaryFunc_
{
    STRUCT_COMP_BASE;

    enum BinaryFuncType type;
    int nb_acks;
    unsigned long long max_event_time;

    int width_res, width_arg1, width_arg2;
    int isSigned_res, isSigned_arg1, isSigned_arg2;

    mpz_t mpint_tmp1, mpint_tmp2, mpint_tmp3, mpint_mask_res;
};

void BrzBinaryFunc_Process_int (struct BrzBinaryFunc_ *c)
{
    int arg1 = GET_PORT_DATA (1);
    int arg2 = GET_PORT_DATA (2);
    int result;
    bool signedOp = c->isSigned_arg1 || c->isSigned_arg2;

    if (c->isSigned_arg1)
        arg1 = EXPAND_INT_SIGN (arg1, c->width_arg1);
    if (c->isSigned_arg2)
        arg2 = EXPAND_INT_SIGN (arg2, c->width_arg2);

    switch (c->type)
    {
    case NotEquals:
        result = (arg1 != arg2);
        break;
    case LessThan:
        if (signedOp)
            result = (((int) arg1) < ((int) arg2));
        else
            result = (((unsigned) arg1) < ((unsigned) arg2));
        break;
    case GreaterThan:
        if (signedOp)
            result = (((int) arg1) > ((int) arg2));
        else
            result = (((unsigned) arg1) > ((unsigned) arg2));
        break;
    case LessOrEquals:
        if (signedOp)
            result = (((int) arg1) <= ((int) arg2));
        else
            result = (((unsigned) arg1) <= ((unsigned) arg2));
        break;
    case GreaterOrEquals:
        if (signedOp)
            result = (((int) arg1) >= ((int) arg2));
        else
            result = (((unsigned) arg1) >= ((unsigned) arg2));
        break;
    case Equals:
        result = (arg1 == arg2);
        break;
    case Add:
        result = (arg1 + arg2);
        break;
    case Subtract:
        result = (arg1 - arg2);
        break;
    case ReverseSubtract:
        result = (arg2 - arg1);
        break;
    case And:
        result = (arg1 & arg2);
        break;
    case Or:
        result = (arg1 | arg2);
        break;
    case Xor:
        result = (arg1 ^ arg2);
        break;
    }

    if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        result &= (1 << c->width_res) - 1;

    SET_PORT_DATA (0, result);
}

void BrzBinaryFunc_Process_mpint (struct BrzBinaryFunc_ *c)
{
    mpz_ptr arg1;
    mpz_ptr arg2;
    int result;

    if (c->width_arg1 > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        arg1 = (mpz_ptr) GET_PORT_DATA (1);
    else
    {
        if (c->isSigned_arg1)
        {
            int tmp = GET_PORT_DATA (1);

            tmp = EXPAND_INT_SIGN (tmp, c->width_arg1);
            mpz_set_si (c->mpint_tmp1, tmp);
        } else
            mpz_set_ui (c->mpint_tmp1, GET_PORT_DATA (1));
        arg1 = c->mpint_tmp1;
    }

    if (c->width_arg2 > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        arg2 = (mpz_ptr) GET_PORT_DATA (2);
    else
    {
        if (c->isSigned_arg2)
        {
            int tmp = GET_PORT_DATA (2);

            tmp = EXPAND_INT_SIGN (tmp, c->width_arg2);
            mpz_set_si (c->mpint_tmp2, tmp);
        } else
            mpz_set_ui (c->mpint_tmp2, GET_PORT_DATA (2));
        arg2 = c->mpint_tmp2;
    }

    switch (c->type)
    {
    case NotEquals:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp != 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case LessThan:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp < 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case GreaterThan:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp > 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case LessOrEquals:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp <= 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case GreaterOrEquals:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp >= 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case Equals:
        {
            int cmp = mpz_cmp (arg1, arg2);

            if (cmp == 0)
                mpz_set_ui (c->mpint_tmp3, 1);
            else
                mpz_set_ui (c->mpint_tmp3, 0);
        }
        break;
    case Add:
        mpz_add (c->mpint_tmp3, arg1, arg2);
        break;
    case Subtract:
        mpz_sub (c->mpint_tmp3, arg1, arg2);
        break;
    case ReverseSubtract:
        mpz_sub (c->mpint_tmp3, arg2, arg1);
        break;
    case And:
        mpz_and (c->mpint_tmp3, arg1, arg2);
        break;
    case Or:
        mpz_ior (c->mpint_tmp3, arg1, arg2);
        break;
    case Xor:
        mpz_xor (c->mpint_tmp3, arg1, arg2);
        break;
    }

    mpz_and (c->mpint_tmp3, c->mpint_tmp3, c->mpint_mask_res); //mask_res=((1<<width_res)-1)

    if (c->width_res > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        result = (long) c->mpint_tmp3;
    } else
    {
        if (c->isSigned_res)
            result = mpz_get_si (c->mpint_tmp3);
        else
            result = mpz_get_ui (c->mpint_tmp3);

        if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            result &= (1 << c->width_res) - 1;
    }

    SET_PORT_DATA (0, result);
}

void BrzBinaryFunc_requp (struct comp *comp, int port)
{
    struct BrzBinaryFunc_ *c = (struct BrzBinaryFunc_ *) comp;

    c->nb_acks = 0;
    c->max_event_time = 0;
    TRACE_NEW_THREAD (1, 0);
    TRACE_NEW_THREAD (2, 0);
    SEND_REQUP (1);
    SEND_REQUP (2);
}

void BrzBinaryFunc_ackup (struct comp *comp, int port)
{
    printf ("BrzBinaryFunc_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzBinaryFunc_ackup_int (struct comp *comp, int port)
{
    struct BrzBinaryFunc_ *c = (struct BrzBinaryFunc_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        BrzBinaryFunc_Process_int (c);

        current_real_time = c->max_event_time;
        SEND_DATAON_DELAYED (0, 0);
        SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
    }
}
void BrzBinaryFunc_ackup_mpint (struct comp *comp, int port)
{
    struct BrzBinaryFunc_ *c = (struct BrzBinaryFunc_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        BrzBinaryFunc_Process_mpint (c);

        current_real_time = c->max_event_time;
        SEND_DATAON_DELAYED (0, 0);
        SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
    }
}
void BrzBinaryFunc_reqdown (struct comp *comp, int port)
{
    struct BrzBinaryFunc_ *c = (struct BrzBinaryFunc_ *) comp;

    c->nb_acks = 0;
    c->max_event_time = 0;
    SEND_REQDOWN (1);
    SEND_REQDOWN (2);
}

void BrzBinaryFunc_ackdown (struct comp *comp, int port)
{
    struct BrzBinaryFunc_ *c = (struct BrzBinaryFunc_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == 2)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (0, 1);
    }
}

//void BrzBinaryFunc (int num, int width_res, int width_arg1, int width_arg2, enum BinaryFuncType type, int isSigned_res, int isSigned_arg1, int isSigned_arg2, int in, int out1, int out2)
void BrzBinaryFunc (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg1 = atoi (args[1]);
    int width_arg2 = atoi (args[2]);
    enum BinaryFuncType type = arg2BinaryFuncType (args[3]);
    int isSigned_res = arg2bool (args[4]);
    int isSigned_arg1 = arg2bool (args[5]);
    int isSigned_arg2 = arg2bool (args[6]);
    int out1 = atoi (args[8]);
    int out2 = atoi (args[9]);

    INIT_COMP (BrzBinaryFunc);
    INIT_COMP_PASSIVE_PORT (BrzBinaryFunc, 0, atoi (args[7]));
    INIT_COMP_ACTIVE_PORT (BrzBinaryFunc, 1, out1);
    INIT_COMP_ACTIVE_PORT (BrzBinaryFunc, 2, out2);
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[7]));

    if (width_arg1 <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2)
      && width_arg2 <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2) && width_res <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2))
    {
        channel[out1].ackup.fct = channel[out2].ackup.fct = BrzBinaryFunc_ackup_int;
    } else
    {
        channel[out1].ackup.fct = channel[out2].ackup.fct = BrzBinaryFunc_ackup_mpint;

        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);
        mpz_init (c->mpint_tmp3);

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }

    c->width_res = width_res;
    c->width_arg1 = width_arg1;
    c->width_arg2 = width_arg2;
    c->isSigned_res = isSigned_res;
    c->isSigned_arg1 = isSigned_arg1;
    c->isSigned_arg2 = isSigned_arg2;
    c->type = type;

    switch (c->type)
    {
    case NotEquals:
        WRITE_DEBUG_INFO_COMP ("_!=_", 3);
        break;
    case LessThan:
        WRITE_DEBUG_INFO_COMP ("_<_", 3);
        break;
    case GreaterThan:
        WRITE_DEBUG_INFO_COMP ("_>_", 3);
        break;
    case LessOrEquals:
        WRITE_DEBUG_INFO_COMP ("_<=_", 3);
        break;
    case GreaterOrEquals:
        WRITE_DEBUG_INFO_COMP ("_>=_", 3);
        break;
    case Equals:
        WRITE_DEBUG_INFO_COMP ("_==_", 3);
        break;
    case Add:
        WRITE_DEBUG_INFO_COMP ("_+_", 3);
        break;
    case Subtract:
        WRITE_DEBUG_INFO_COMP ("_-_", 3);
        break;
    case ReverseSubtract:
        WRITE_DEBUG_INFO_COMP ("_\\-_", 3);
        break;
    case And:
        WRITE_DEBUG_INFO_COMP ("_&_", 3);
        break;
    case Or:
        WRITE_DEBUG_INFO_COMP ("_|_", 3);
        break;
    case Xor:
        WRITE_DEBUG_INFO_COMP ("_^_", 3);
        break;
    default:
        WRITE_DEBUG_INFO_COMP ("BinaryFunc", 3);
    }
}

/****************************************************/

#define BrzBinaryFuncPush_ BrzBinaryFunc_

void BrzBinaryFuncPush_requp (struct comp *comp, int port)
{
    printf ("BrzBinaryFuncPush_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzBinaryFuncPush_requp_int (struct comp *comp, int port)
{
    struct BrzBinaryFuncPush_ *c = (struct BrzBinaryFuncPush_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        BrzBinaryFunc_Process_int (c);
        current_real_time = c->max_event_time;
        SEND_DATAON_DELAYED (0, 0);
        SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
    }
}
void BrzBinaryFuncPush_requp_mpint (struct comp *comp, int port)
{
    struct BrzBinaryFuncPush_ *c = (struct BrzBinaryFuncPush_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        BrzBinaryFunc_Process_mpint (c);
        current_real_time = c->max_event_time;
        SEND_DATAON_DELAYED (0, 0);
        SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
    }
}
void BrzBinaryFuncPush_ackup (struct comp *comp, int port)
{
    struct BrzBinaryFuncPush_ *c = (struct BrzBinaryFuncPush_ *) comp;

    c->nb_acks = 0;
    c->max_event_time = 0;
    SEND_ACKUP (1);
    SEND_ACKUP (2);
}

void BrzBinaryFuncPush_reqdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncPush_ *c = (struct BrzBinaryFuncPush_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == 2)
    {
        current_real_time = c->max_event_time;
        SEND_REQDOWN (0);
    }
}
void BrzBinaryFuncPush_ackdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncPush_ *c = (struct BrzBinaryFuncPush_ *) comp;

    c->nb_acks = 0;
    c->max_event_time = 0;
    SEND_ACKDOWN_DELAYED (1, 0);
    SEND_ACKDOWN_DELAYED (2, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzBinaryFuncPush (int num, int width_res, int width_arg1, int width_arg2, enum BinaryFuncPushType type, int isSigned_res, int isSigned_arg1, int isSigned_arg2, int in, int out1, int out2)
void BrzBinaryFuncPush (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg1 = atoi (args[1]);
    int width_arg2 = atoi (args[2]);
    enum BinaryFuncType type = arg2BinaryFuncType (args[3]);
    int isSigned_res = arg2bool (args[4]);
    int isSigned_arg1 = arg2bool (args[5]);
    int isSigned_arg2 = arg2bool (args[6]);
    int out1 = atoi (args[8]);
    int out2 = atoi (args[9]);

    INIT_COMP (BrzBinaryFuncPush);
    INIT_COMP_ACTIVE_PORT (BrzBinaryFuncPush, 0, atoi (args[7]));
    INIT_COMP_PASSIVE_PORT (BrzBinaryFuncPush, 1, out1);
    INIT_COMP_PASSIVE_PORT (BrzBinaryFuncPush, 2, out2);

    if (width_arg1 <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2)
      && width_arg2 <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2) && width_res <= ((HOST_INTEGER_WIDTH_IN_BYTES * 8) - 2))
    {
        channel[out1].requp.fct = channel[out2].requp.fct = BrzBinaryFuncPush_requp_int;
    } else
    {
        channel[out1].requp.fct = channel[out2].requp.fct = BrzBinaryFuncPush_requp_mpint;

        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);
        mpz_init (c->mpint_tmp3);

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }

    c->width_res = width_res;
    c->width_arg1 = width_arg1;
    c->width_arg2 = width_arg2;
    c->isSigned_res = isSigned_res;
    c->isSigned_arg1 = isSigned_arg1;
    c->isSigned_arg2 = isSigned_arg2;
    c->type = type;

    switch (c->type)
    {
    case NotEquals:
        WRITE_DEBUG_INFO_COMP ("_!=_", 3);
        break;
    case LessThan:
        WRITE_DEBUG_INFO_COMP ("_<_", 3);
        break;
    case GreaterThan:
        WRITE_DEBUG_INFO_COMP ("_>_", 3);
        break;
    case LessOrEquals:
        WRITE_DEBUG_INFO_COMP ("_<=_", 3);
        break;
    case GreaterOrEquals:
        WRITE_DEBUG_INFO_COMP ("_>=_", 3);
        break;
    case Equals:
        WRITE_DEBUG_INFO_COMP ("_==_", 3);
        break;
    case Add:
        WRITE_DEBUG_INFO_COMP ("_+_", 3);
        break;
    case Subtract:
        WRITE_DEBUG_INFO_COMP ("_-_", 3);
        break;
    case ReverseSubtract:
        WRITE_DEBUG_INFO_COMP ("_\\-_", 3);
        break;
    case And:
        WRITE_DEBUG_INFO_COMP ("_&_", 3);
        break;
    case Or:
        WRITE_DEBUG_INFO_COMP ("_|_", 3);
        break;
    case Xor:
        WRITE_DEBUG_INFO_COMP ("_^_", 3);
        break;
    default:
        WRITE_DEBUG_INFO_COMP ("BinaryFuncPush", 3);
    }
}

/****************************************************/

struct BrzBinaryFuncConstR_
{
    STRUCT_COMP_BASE;

    unsigned long constant;
    enum BinaryFuncType type;

    int width_arg1;
    int width_res;
    int isSigned_arg1;
    int isSigned_res;

    mpz_t mpint_tmp1, mpint_tmp2;
    mpz_t mpint_mask_res;
};

int BrzBinaryFuncConstR_Process_int (struct BrzBinaryFuncConstR_ *c, int arg1)
{
    int result;

    if (c->isSigned_arg1)
        arg1 = EXPAND_INT_SIGN (arg1, c->width_arg1);

    switch (c->type)
    {
    case NotEquals:
        result = (arg1 != c->constant);
        break;
    case LessThan:
        if (c->isSigned_arg1)
            result = (((int) arg1) < ((int) c->constant));
        else
            result = (((unsigned) arg1) < ((unsigned) c->constant));
        break;
    case GreaterThan:
        if (c->isSigned_arg1)
            result = (((int) arg1) > ((int) c->constant));
        else
            result = (((unsigned) arg1) > ((unsigned) c->constant));
        break;
    case LessOrEquals:
        if (c->isSigned_arg1)
            result = (((int) arg1) <= ((int) c->constant));
        else
            result = (((unsigned) arg1) <= ((unsigned) c->constant));
        break;
    case GreaterOrEquals:
        if (c->isSigned_arg1)
            result = (((int) arg1) >= ((int) c->constant));
        else
            result = (((unsigned) arg1) >= ((unsigned) c->constant));
        break;
    case Equals:
        result = (arg1 == c->constant);
        break;
    case Add:
        result = (arg1 + c->constant);
        break;
    case Subtract:
        result = (arg1 - c->constant);
        break;
    case ReverseSubtract:
        result = (c->constant - arg1);
        break;
    case And:
        result = (arg1 & c->constant);
        break;
    case Or:
        result = (arg1 | c->constant);
        break;
    case Xor:
        result = (arg1 ^ c->constant);
        break;
    }

    if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        result &= (1 << c->width_res) - 1;

    return result;
}

/* RecoverMPZSign : recover the sign of a 2's number held in an mpz_t with
	the sign (top) bit at index `width'-1 */
void RecoverMPZSign (mpz_ptr num, int width)
{
    mpz_t tmp;

    mpz_setbit (num, width);    /* Place a guard set bit and then scan test to find
                                   the sign bit */

    if (mpz_scan1 (num, width - 1) == (width - 1))
    {
        mpz_init (tmp);
        mpz_setbit (tmp, width);
        mpz_sub_ui (tmp, tmp, 1); /* 0001111111 mask for width */
        mpz_com (tmp, tmp);     /* 1110000000 sign extend for width */
        mpz_ior (num, num, tmp);
        mpz_clear (tmp);
    } else
        mpz_clrbit (num, width);
}

int BrzBinaryFuncConstR_Process_mpint (struct BrzBinaryFuncConstR_ *c, mpz_ptr arg1)
{
    int result;

    if (c->width_arg1 > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        if (c->isSigned_arg1)
        {
            arg1 = (mpz_ptr) GET_PORT_DATA (1);
            mpz_set (c->mpint_tmp1, arg1);
            RecoverMPZSign (c->mpint_tmp1, c->width_arg1);
            arg1 = c->mpint_tmp1;
        }
    } else
    {
        if (c->isSigned_arg1)
        {
            int tmp = (long) arg1;

            tmp = EXPAND_INT_SIGN (tmp, c->width_arg1);
            mpz_set_si (c->mpint_tmp1, tmp);
        } else
            mpz_set_ui (c->mpint_tmp1, GET_PORT_DATA (1));
        arg1 = c->mpint_tmp1;
    }

    switch (c->type)
    {
    case NotEquals:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp != 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case LessThan:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp < 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case GreaterThan:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp > 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case LessOrEquals:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp <= 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case GreaterOrEquals:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp >= 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case Equals:
        {
            int cmp = mpz_cmp (arg1, (mpz_ptr) c->constant);

            if (cmp == 0)
                mpz_set_ui (c->mpint_tmp2, 1);
            else
                mpz_set_ui (c->mpint_tmp2, 0);
        }
        break;
    case Add:
        mpz_add (c->mpint_tmp2, arg1, (mpz_ptr) c->constant);
        break;
    case Subtract:
        mpz_sub (c->mpint_tmp2, arg1, (mpz_ptr) c->constant);
        break;
    case ReverseSubtract:
        mpz_sub (c->mpint_tmp2, (mpz_ptr) c->constant, arg1);
        break;
    case And:
        mpz_and (c->mpint_tmp2, arg1, (mpz_ptr) c->constant);
        break;
    case Or:
        mpz_ior (c->mpint_tmp2, arg1, (mpz_ptr) c->constant);
        break;
    case Xor:
        mpz_xor (c->mpint_tmp2, arg1, (mpz_ptr) c->constant);
        break;
    }

    mpz_and (c->mpint_tmp2, c->mpint_tmp2, c->mpint_mask_res); //mask_res=((1<<width_res)-1)

    if (c->width_res > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        result = (long) c->mpint_tmp2;
    } else
    {
        if (c->isSigned_res)
            result = mpz_get_si (c->mpint_tmp2);
        else
            result = mpz_get_ui (c->mpint_tmp2);

        if (c->width_res < (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            result &= (1 << c->width_res) - 1;
    }

    return result;
}

void BrzBinaryFuncConstR_requp (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    TRACE_FOLLOW_THREAD (1, 0);
    SEND_REQUP (1);
}

void BrzBinaryFuncConstR_ackup (struct comp *comp, int port)
{
    printf ("BrzBinaryFuncConstR_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzBinaryFuncConstR_ackup_int (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    int arg1 = GET_PORT_DATA (1);
    int result = BrzBinaryFuncConstR_Process_int (c, arg1);

    SET_PORT_DATA (0, result);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzBinaryFuncConstR_ackup_mpint (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    mpz_ptr arg1 = (mpz_ptr) GET_PORT_DATA (1);
    int result = BrzBinaryFuncConstR_Process_mpint (c, arg1);

    SET_PORT_DATA (0, result);
    SEND_DATAON_DELAYED (0, 0);
    SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzBinaryFuncConstR_reqdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    SEND_REQDOWN (1);
}

void BrzBinaryFuncConstR_ackdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    SEND_ACKDOWN_DELAYED (0, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzBinaryFuncConstR (int num, int width_res, int width_arg1, int width_arg2, enum BinaryFuncType type, int isSigned_res, int isSigned_arg1, int isSigned_arg2, unsigned int constant, int in, int out1)
void BrzBinaryFuncConstR (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg1 = atoi (args[1]);
    int width_arg2 = atoi (args[2]);
    enum BinaryFuncType type = arg2BinaryFuncType (args[3]);
    int isSigned_res = arg2bool (args[4]);
    int isSigned_arg1 = arg2bool (args[5]);
    int isSigned_arg2 = arg2bool (args[6]);
    char *constantStr = (char *) args[7];
    int out1 = atoi (args[9]);

    INIT_COMP (BrzBinaryFuncConstR);
    INIT_COMP_PASSIVE_PORT (BrzBinaryFuncConstR, 0, atoi (args[8]));
    INIT_COMP_ACTIVE_PORT (BrzBinaryFuncConstR, 1, out1);
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[8]));

    if (width_arg1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)
      && width_arg2 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8) && width_res <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        int constant;

        sscanf (constantStr, "%u", &constant);

        channel[out1].ackup.fct = BrzBinaryFuncConstR_ackup_int;

        if (isSigned_arg2)
            c->constant = EXPAND_INT_SIGN (constant, width_arg2);
        else
            c->constant = constant;
    } else
    {
        channel[out1].ackup.fct = BrzBinaryFuncConstR_ackup_mpint;

        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);

        {
            c->constant = (long) malloc (sizeof (mpz_t));
            mpz_init ((mpz_ptr) c->constant);
            mpz_set_str ((mpz_ptr) c->constant, constantStr, 10);

            if (isSigned_arg2)
            {
                // fprintf (stderr, "Error in BrzBinaryFuncConstR: signed constant\n");
                RecoverMPZSign ((mpz_ptr) c->constant, width_arg2);
                // mpz_out_str (stderr, 10, (mpz_ptr) c->constant);
                // CloseBreezeSimulation (2);
            }
        }

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }

    c->type = type;
    c->width_arg1 = width_arg1;
    c->width_res = width_res;
    c->isSigned_arg1 = isSigned_arg1;
    c->isSigned_res = isSigned_res;

    {
        char *desc;

        switch (c->type)
        {
        case NotEquals:
            desc = g_strdup_printf ("\"_!=%s\"", constantStr);
            break;
        case LessThan:
            desc = g_strdup_printf ("\"_<%s\"", constantStr);
            break;
        case GreaterThan:
            desc = g_strdup_printf ("\"_>%s\"", constantStr);
            break;
        case LessOrEquals:
            desc = g_strdup_printf ("\"_<=%s\"", constantStr);
            break;
        case GreaterOrEquals:
            desc = g_strdup_printf ("\"_>=%s\"", constantStr);
            break;
        case Equals:
            desc = g_strdup_printf ("\"_==%s\"", constantStr);
            break;
        case Add:
            desc = g_strdup_printf ("\"_+%s\"", constantStr);
            break;
        case Subtract:
            desc = g_strdup_printf ("\"_-%s\"", constantStr);
            break;
        case ReverseSubtract:
            desc = g_strdup_printf ("\"%s-_\"", constantStr);
            break;
        case And:
            desc = g_strdup_printf ("\"_&%s\"", constantStr);
            break;
        case Or:
            desc = g_strdup_printf ("\"_|%s\"", constantStr);
            break;
        case Xor:
            desc = g_strdup_printf ("\"_^%s\"", constantStr);
            break;
        default:
            desc = g_strdup_printf ("\"_???%s\"", constantStr);
        }

        WRITE_DEBUG_INFO_COMP (desc, 2);
        free (desc);
    }
}

/****************************************************/

#define BrzBinaryFuncConstRPush_ BrzBinaryFuncConstR_

void BrzBinaryFuncConstRPush_requp (struct comp *comp, int port)
{
    printf ("BrzBinaryFuncConstRPush_ackup: This code should never be reached\n");
    CloseBreezeSimulation (2);
}

void BrzBinaryFuncConstRPush_requp_int (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    int arg1 = GET_PORT_DATA (1);
    int result = BrzBinaryFuncConstR_Process_int (c, arg1);

    SET_PORT_DATA (0, result);
    TRACE_FOLLOW_THREAD (0, 1);
    SEND_DATAON_DELAYED (0, 0);
    SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzBinaryFuncConstRPush_requp_mpint (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    mpz_ptr arg1 = (mpz_ptr) GET_PORT_DATA (1);
    int result = BrzBinaryFuncConstR_Process_mpint (c, arg1);

    SET_PORT_DATA (0, result);
    TRACE_FOLLOW_THREAD (0, 1);
    SEND_DATAON_DELAYED (0, 0);
    SEND_REQUP_DELAYED (0, BREEZE_DATAPATH_DELAY);
}

void BrzBinaryFuncConstRPush_ackup (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    SEND_ACKUP (1);
}

void BrzBinaryFuncConstRPush_reqdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    SEND_REQDOWN_DELAYED (0, 0);
}

void BrzBinaryFuncConstRPush_ackdown (struct comp *comp, int port)
{
    struct BrzBinaryFuncConstR_ *c = (struct BrzBinaryFuncConstR_ *) comp;

    SEND_ACKDOWN_DELAYED (1, 0);
    SEND_DATAOFF_DELAYED (0, 1);
}

//void BrzBinaryFuncConstRPush (int num, int width_res, int width_arg1, int width_arg2, enum BinaryFuncType type, int isSigned_res, int isSigned_arg1, int isSigned_arg2, unsigned int constant, int in, int out1)
void BrzBinaryFuncConstRPush (int num, char *args[])
{
    int width_res = atoi (args[0]);
    int width_arg1 = atoi (args[1]);
    int width_arg2 = atoi (args[2]);
    enum BinaryFuncType type = arg2BinaryFuncType (args[3]);
    int isSigned_res = arg2bool (args[4]);
    int isSigned_arg1 = arg2bool (args[5]);
    int isSigned_arg2 = arg2bool (args[6]);
    char *constantStr = (char *) args[7];
    int out1 = atoi (args[9]);

    INIT_COMP (BrzBinaryFuncConstRPush);
    INIT_COMP_ACTIVE_PORT (BrzBinaryFuncConstRPush, 0, atoi (args[8]));
    INIT_COMP_PASSIVE_PORT (BrzBinaryFuncConstRPush, 1, out1);
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[8]));

    if (width_arg1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8)
      && width_arg2 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8) && width_res <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        int constant;

        sscanf (constantStr, "%u", &constant);

        channel[out1].requp.fct = BrzBinaryFuncConstRPush_requp_int;

        if (isSigned_arg2)
            c->constant = EXPAND_INT_SIGN (constant, width_arg2);
        else
            c->constant = constant;
    } else
    {
        channel[out1].requp.fct = BrzBinaryFuncConstRPush_requp_mpint;

        if (width_arg1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_init (c->mpint_tmp1);
        }

        mpz_init (c->mpint_tmp2);

        {
            c->constant = (long) malloc (sizeof (mpz_t));
            mpz_init ((mpz_ptr) c->constant);
            mpz_set_str ((mpz_ptr) c->constant, constantStr, 10);

            if (isSigned_arg2)
            {
                RecoverMPZSign ((mpz_ptr) c->constant, width_arg2);
                /*
                   fprintf (stderr, "Error in BrzBinaryFuncConstRPush: signed constant\n");
                   CloseBreezeSimulation (2);
                 */
            }
        }

        //mask_res=((1<<width_res)-1)
        mpz_init (c->mpint_mask_res);
        mpz_setbit (c->mpint_mask_res, width_res);
        mpz_sub_ui (c->mpint_mask_res, c->mpint_mask_res, 1);
    }

    c->type = type;
    c->width_arg1 = width_arg1;
    c->width_res = width_res;
    c->isSigned_arg1 = isSigned_arg1;
    c->isSigned_res = isSigned_res;

    {
        char *desc;

        switch (c->type)
        {
        case NotEquals:
            desc = g_strdup_printf ("\"_!=%s\"", constantStr);
            break;
        case LessThan:
            desc = g_strdup_printf ("\"_<%s\"", constantStr);
            break;
        case GreaterThan:
            desc = g_strdup_printf ("\"_>%s\"", constantStr);
            break;
        case LessOrEquals:
            desc = g_strdup_printf ("\"_<=%s\"", constantStr);
            break;
        case GreaterOrEquals:
            desc = g_strdup_printf ("\"_>=%s\"", constantStr);
            break;
        case Equals:
            desc = g_strdup_printf ("\"_==%s\"", constantStr);
            break;
        case Add:
            desc = g_strdup_printf ("\"_+%s\"", constantStr);
            break;
        case Subtract:
            desc = g_strdup_printf ("\"_-%s\"", constantStr);
            break;
        case ReverseSubtract:
            desc = g_strdup_printf ("\"%s-_\"", constantStr);
            break;
        case And:
            desc = g_strdup_printf ("\"_&%s\"", constantStr);
            break;
        case Or:
            desc = g_strdup_printf ("\"_|%s\"", constantStr);
            break;
        case Xor:
            desc = g_strdup_printf ("\"_^%s\"", constantStr);
            break;
        default:
            desc = g_strdup_printf ("\"_???%s\"", constantStr);
        }

        WRITE_DEBUG_INFO_COMP (desc, 2);
        free (desc);
    }
}

/****************************************************/

struct BrzCombineEqual_
{
    STRUCT_COMP_BASE;

    int width_in;
    int width_out;
    int nb_outputs;

    int nb_acks;
    unsigned long long max_event_time;

    mpz_t mpint_tmp;
    mpz_t mpint_result;
};

void BrzCombineEqual_requp (struct comp *comp, int port)
{
    int i;
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;

    for (i = c->nb_outputs; i > 0; i--)
    {
        TRACE_NEW_THREAD (i, 0);
        SEND_REQUP_DELAYED (i, 0 /*(i - 1) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acks = 0;
    c->max_event_time = 0;
}

int BrzCombineEqual_ackup_int_int (struct comp *comp, int port)
{
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;
    int result;
    int i;

    result = 0;
    for (i = 0; i < c->nb_outputs; i++)
    {
        result |= GET_PORT_DATA (i + 1) << (i * c->width_out);
    }
    return result;
}

long BrzCombineEqual_ackup_int_mpint (struct comp *comp, int port)
{
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;
    long result = (long) c->mpint_result;
    int i;

    mpz_set_ui ((mpz_ptr) result, 0);
    for (i = 0; i < c->nb_outputs; i++)
    {
        mpz_set_ui (c->mpint_tmp, GET_PORT_DATA (i + 1));
        mpz_mul_2exp (c->mpint_tmp, c->mpint_tmp, i * c->width_out);
        mpz_ior ((mpz_ptr) result, (mpz_ptr) result, c->mpint_tmp);
    }
    return result;
}

long BrzCombineEqual_ackup_mpint_mpint (struct comp *comp, int port)
{
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;
    long result = (long) c->mpint_result;
    int i;

    mpz_set_ui ((mpz_ptr) result, 0);
    for (i = 0; i < c->nb_outputs; i++)
    {
        mpz_set (c->mpint_tmp, (mpz_ptr) GET_PORT_DATA (i + 1));
        mpz_mul_2exp (c->mpint_tmp, c->mpint_tmp, i * c->width_out);
        mpz_ior ((mpz_ptr) result, (mpz_ptr) result, c->mpint_tmp);
    }
    return result;
}

void BrzCombineEqual_ackup (struct comp *comp, int port)
{
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == c->nb_outputs)
    {
        int result;

        if (c->width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            result = BrzCombineEqual_ackup_int_int (comp, port);
        } else
        {
            if (c->width_out <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
                result = BrzCombineEqual_ackup_int_mpint (comp, port);
            else
                result = BrzCombineEqual_ackup_mpint_mpint (comp, port);
        }

        current_real_time = c->max_event_time;
        SET_PORT_DATA (0, result);
        SEND_DATAON_DELAYED (0, 0);
        SEND_ACKUP_DELAYED (0, BREEZE_DATAPATH_DELAY + 3 * BREEZE_CONTROL_DELAY);
    }
}
void BrzCombineEqual_reqdown (struct comp *comp, int port)
{
    int i;
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;

    for (i = c->nb_outputs; i > 0; i--)
    {
        SEND_REQDOWN_DELAYED (i, 0 /*i * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acks = 0;
    c->max_event_time = 0;
}

void BrzCombineEqual_ackdown (struct comp *comp, int port)
{
    struct BrzCombineEqual_ *c = (struct BrzCombineEqual_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == c->nb_outputs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 3 * BREEZE_CONTROL_DELAY);
        SEND_DATAOFF_DELAYED (0, (3 * BREEZE_CONTROL_DELAY) + 1);
    }
}

//void BrzCombineEqual (int num, int width_in, int width_out, int nb_outputs, int in, ...)
void BrzCombineEqual (int num, char *args[])
{
    int width_in = atoi (args[0]);
    int width_out = atoi (args[1]);
    int nb_outputs = atoi (args[2]);
    int i;

    INIT_COMP (BrzCombineEqual);
    INIT_COMP_PASSIVE_PORT (BrzCombineEqual, 0, atoi (args[3]));
    INIT_COMP_CONTROLLED_DATA_PORT (0, atoi (args[3]));

    for (i = 0; i < nb_outputs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzCombineEqual, 1 + i, atoi (args[i + 4]));
    }

    c->width_in = width_in;
    c->width_out = width_out;
    c->nb_outputs = nb_outputs;

    mpz_init (c->mpint_tmp);
    mpz_init (c->mpint_result);

    WRITE_DEBUG_INFO_COMP ("CombineEqual", 1 + nb_outputs);
}

/****************************************************/

struct BrzSplit_
{
    STRUCT_COMP_BASE;

    int width_in;
    int width_out1;
    int width_out2;
    int nb_acks;
    unsigned long long max_event_time;

    mpz_t mpint_tmp;
};

void BrzSplit_requp (struct comp *comp, int port)
{
    struct BrzSplit_ *c = (struct BrzSplit_ *) comp;

    long arg = GET_PORT_DATA (0);

    if (c->width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        SET_PORT_DATA (2, ((unsigned int) arg) >> c->width_out1);
        SET_PORT_DATA (1, arg & ((1 << c->width_out1) - 1));
    } else
    {
        if (c->width_out2 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_tdiv_q_2exp (c->mpint_tmp, (mpz_ptr) arg, c->width_out1);
            SET_PORT_DATA (2, mpz_get_ui (c->mpint_tmp));
        } else
        {
            mpz_tdiv_q_2exp ((mpz_ptr) GET_PORT_DATA (2), (mpz_ptr) arg, c->width_out1);
            FAKE_SET_PORT_DATA_FOR_DEBUG_MPINT (2);
        }

        if (c->width_out1 <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_tdiv_r_2exp (c->mpint_tmp, (mpz_ptr) arg, c->width_out1);
            SET_PORT_DATA (1, mpz_get_ui (c->mpint_tmp));
        } else
        {
            mpz_tdiv_r_2exp ((mpz_ptr) GET_PORT_DATA (1), (mpz_ptr) arg, c->width_out1);
            FAKE_SET_PORT_DATA_FOR_DEBUG_MPINT (1);
        }
    }

    TRACE_NEW_THREAD (1, 0);
    TRACE_NEW_THREAD (2, 0);
    SEND_DATAON_DELAYED (1, 0);
    SEND_DATAON_DELAYED (2, 0);
    SEND_REQUP_DELAYED (1, 1);
    SEND_REQUP_DELAYED (2, 1);
    c->nb_acks = 0;
    c->max_event_time = 0;
}

void BrzSplit_ackup (struct comp *comp, int port)
{
    struct BrzSplit_ *c = (struct BrzSplit_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}
void BrzSplit_reqdown (struct comp *comp, int port)
{
    struct BrzSplit_ *c = (struct BrzSplit_ *) comp;

    SEND_REQDOWN_DELAYED (1, 0);
    SEND_REQDOWN_DELAYED (2, 0);
    c->nb_acks = 0;
    c->max_event_time = 0;
}

void BrzSplit_ackdown (struct comp *comp, int port)
{
    struct BrzSplit_ *c = (struct BrzSplit_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);

    if (c->nb_acks == 2)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);
        SEND_DATAOFF_DELAYED (1, 1);
        SEND_DATAOFF_DELAYED (2, 1);
    }
}

//void BrzSplit (int num, int width_in, int width_out1, int width_out2, int in, int out1, int out2)
void BrzSplit (int num, char *args[])
{
    int width_in = atoi (args[0]);
    int width_out1 = atoi (args[1]);
    int width_out2 = atoi (args[2]);

    INIT_COMP (BrzSplit);
    INIT_COMP_PASSIVE_PORT (BrzSplit, 0, atoi (args[3]));
    INIT_COMP_ACTIVE_PORT (BrzSplit, 1, atoi (args[4]));
    INIT_COMP_ACTIVE_PORT (BrzSplit, 2, atoi (args[5]));
    INIT_COMP_CONTROLLED_DATA_PORT (1, atoi (args[4]));
    INIT_COMP_CONTROLLED_DATA_PORT (2, atoi (args[5]));

    if (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        mpz_init (c->mpint_tmp);
    if (width_out1 > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (1) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (1));
    }
    if (width_out2 > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        GET_PORT_DATA (2) = (long) malloc (sizeof (mpz_t));
        mpz_init ((mpz_ptr) GET_PORT_DATA (2));
    }

    if (width_out1 + width_out2 != width_in)
    {
        fprintf (stderr, "BrzSplit: width_out1+width_out2 != width_in");
        CloseBreezeSimulation (2);
    }
    c->width_in = width_in;
    c->width_out1 = width_out1;
    c->width_out2 = width_out2;

    WRITE_DEBUG_INFO_COMP ("Split", 3);
}

/****************************************************/

struct BrzSplitEqual_
{
    STRUCT_COMP_BASE;

    int width_in;
    int width_out;
    int nb_outputs;

    int nb_acks;
    unsigned long long max_event_time;

    mpz_t mpint_tmp1, mpint_tmp2;
};

void BrzSplitEqual_requp (struct comp *comp, int port)
{
    int i;
    struct BrzSplitEqual_ *c = (struct BrzSplitEqual_ *) comp;

    long arg = GET_PORT_DATA (0);

    if (c->width_in <= (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        int data = arg;
        int mask = (1 << c->width_out) - 1;

        for (i = 0; i < c->nb_outputs; i++)
        {
            SET_PORT_DATA (i + 1, data & mask);
            data = ((unsigned int) data) >> c->width_out;
        }
    } else
    {
        if (c->width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
        {
            mpz_set (c->mpint_tmp1, (mpz_ptr) arg);
            for (i = 0; i < c->nb_outputs; i++)
            {
                mpz_tdiv_r_2exp (c->mpint_tmp2, c->mpint_tmp1, c->width_out);
                mpz_set ((mpz_ptr) GET_PORT_DATA (i + 1), c->mpint_tmp2);
                mpz_tdiv_q_2exp (c->mpint_tmp1, c->mpint_tmp1, c->width_out);
            }
        } else
        {
            mpz_set (c->mpint_tmp1, (mpz_ptr) arg);
            for (i = 0; i < c->nb_outputs; i++)
            {
                mpz_tdiv_r_2exp (c->mpint_tmp2, c->mpint_tmp1, c->width_out);
                SET_PORT_DATA (i + 1, mpz_get_ui (c->mpint_tmp2));
                mpz_tdiv_q_2exp (c->mpint_tmp1, c->mpint_tmp1, c->width_out);
            }
        }
    }

    for (i = c->nb_outputs; i > 0; i--)
    {
        TRACE_NEW_THREAD (i, 0);
        SEND_DATAON_DELAYED (i, 0);
        SEND_REQUP_DELAYED (i, 1);
    }

    c->nb_acks = 0;
    c->max_event_time = 0;
}

void BrzSplitEqual_ackup (struct comp *comp, int port)
{
    struct BrzSplitEqual_ *c = (struct BrzSplitEqual_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == c->nb_outputs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}
void BrzSplitEqual_reqdown (struct comp *comp, int port)
{
    int i;
    struct BrzSplitEqual_ *c = (struct BrzSplitEqual_ *) comp;

    for (i = c->nb_outputs; i > 0; i--)
    {
        SEND_REQDOWN_DELAYED (i, 1);
    }
    c->nb_acks = 0;
    c->max_event_time = 0;
}

void BrzSplitEqual_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzSplitEqual_ *c = (struct BrzSplitEqual_ *) comp;

    c->nb_acks++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acks == c->nb_outputs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);
	    for (i = c->nb_outputs; i > 0; i--)
	    {
    	    SEND_DATAOFF_DELAYED (i, 1);
		}
    }
}

//void BrzSplitEqual (int num, int width_in, int width_out, int nb_outputs, int in, ...)
void BrzSplitEqual (int num, char *args[])
{
    int width_in = atoi (args[0]);
    int width_out = atoi (args[1]);
    int nb_outputs = atoi (args[2]);
    int i;

    INIT_COMP (BrzSplitEqual);
    INIT_COMP_PASSIVE_PORT (BrzSplitEqual, 0, atoi (args[3]));

    for (i = 0; i < nb_outputs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzSplitEqual, 1 + i, atoi (args[i + 4]));
        INIT_COMP_CONTROLLED_DATA_PORT (1 + i, atoi (args[i + 4]));
    }

    c->width_in = width_in;
    c->width_out = width_out;
    c->nb_outputs = nb_outputs;

    if (width_in > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        mpz_init (c->mpint_tmp1);
        mpz_init (c->mpint_tmp2);
    }

    if (width_out > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        for (i = 0; i < nb_outputs; i++)
        {
            GET_PORT_DATA (i + 1) = (long) malloc (sizeof (mpz_t));
            mpz_init ((mpz_ptr) GET_PORT_DATA (i + 1));
        }
    }

    WRITE_DEBUG_INFO_COMP ("SplitEqual", 1 + nb_outputs);
}

/****************************************************/

struct BrzSynchPush_
{
    STRUCT_COMP_BASE;

    int nb_inputs;
    int nb_reqs;
    unsigned long long max_event_time;
};

void BrzSynchPush_requp (struct comp *comp, int port)
{
    int i;
    struct BrzSynchPush_ *c = (struct BrzSynchPush_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs + 1)
    {
        current_real_time = c->max_event_time;
        for (i = c->nb_inputs; i >= 1; i--)
        {
            SET_PORT_DATA (i, GET_PORT_DATA (0));
            SEND_DATAON_DELAYED (i, 0);
        }
        SET_PORT_DATA (1 + c->nb_inputs, GET_PORT_DATA (0));
        TRACE_FOLLOW_THREAD (1 + c->nb_inputs, 0);
        SEND_DATAON_DELAYED (1 + c->nb_inputs, 0);
        SEND_REQUP_DELAYED (1 + c->nb_inputs, 1);
    }
}
void BrzSynchPush_ackup (struct comp *comp, int port)
{
    int i;
    struct BrzSynchPush_ *c = (struct BrzSynchPush_ *) comp;

    for (i = c->nb_inputs; i >= 0; i--)
    {
        SEND_ACKUP (i);
    }
    c->nb_reqs = 0;
    c->max_event_time = 0;
}

void BrzSynchPush_reqdown (struct comp *comp, int port)
{
    struct BrzSynchPush_ *c = (struct BrzSynchPush_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs + 1)
    {
        current_real_time = c->max_event_time;
        SEND_REQDOWN_DELAYED (1 + c->nb_inputs, 1);
    }
}
void BrzSynchPush_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzSynchPush_ *c = (struct BrzSynchPush_ *) comp;

    for (i = c->nb_inputs; i >= 0; i--)
    {
        SEND_ACKDOWN_DELAYED (i, 0);
        SEND_DATAOFF_DELAYED (i, 1);
    }
    SEND_DATAOFF_DELAYED (1 + c->nb_inputs, 1);

    c->nb_reqs = 0;
    c->max_event_time = 0;
}

//void BrzSynchPush (int num, int width, int nb_inputs, int in0, ...) // n in, 1 out
void BrzSynchPush (int num, char *args[])
{
    //    int width = atoi (args[0]);
    int nb_inputs = atoi (args[1]);
    int i;

    INIT_COMP (BrzSynchPush);
    INIT_COMP_PASSIVE_PORT (BrzSynchPush, 0, atoi (args[2]));

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzSynchPush, 1 + i, atoi (args[i + 3]));
        INIT_COMP_CONTROLLED_DATA_PORT (1 + i, atoi (args[i + 3]));
    }

    INIT_COMP_ACTIVE_PORT (BrzSynchPush, 1 + nb_inputs, atoi (args[nb_inputs + 3]));
    INIT_COMP_CONTROLLED_DATA_PORT (1 + nb_inputs, atoi (args[nb_inputs + 3]));

    c->nb_inputs = nb_inputs;
    c->nb_reqs = 0;
    c->max_event_time = 0;

    WRITE_DEBUG_INFO_COMP ("SynchPush", c->nb_inputs);
}

/****************************************************/

struct BrzForkPush_
{
    STRUCT_COMP_BASE;
    int nb_outs;
    int nb_acknowledges;
    unsigned long long max_event_time;
};

void BrzForkPush_requp (struct comp *comp, int port)
{
    int i;
    struct BrzForkPush_ *c = (struct BrzForkPush_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        COPY_DATA_FROM_TO (0, i + 1);
        TRACE_NEW_THREAD (i + 1, 0);
        SEND_DATAON_DELAYED (i + 1, 0);
        SEND_REQUP_DELAYED (i + 1, 1 /*(i + 2) * BREEZE_CONTROL_DELAY */ );
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzForkPush_ackup (struct comp *comp, int port)
{
    struct BrzForkPush_ *c = (struct BrzForkPush_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKUP (0);
    }
}
void BrzForkPush_reqdown (struct comp *comp, int port)
{
    int i;
    struct BrzForkPush_ *c = (struct BrzForkPush_ *) comp;

    for (i = c->nb_outs - 1; i >= 0; i--)
    {
        SEND_REQDOWN_DELAYED (i + 1, 0);
    }
    c->nb_acknowledges = 0;
    c->max_event_time = 0;
}

void BrzForkPush_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzForkPush_ *c = (struct BrzForkPush_ *) comp;

    c->nb_acknowledges++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_acknowledges == c->nb_outs)
    {
        current_real_time = c->max_event_time;
        SEND_ACKDOWN_DELAYED (0, 0);;
        for (i = c->nb_outs - 1; i >= 0; i--)
        {
            SEND_DATAOFF_DELAYED (i + 1, 1);
        }
    }
}

//void BrzForkPush (int num, int width, int nb_outs, int in, ...)
void BrzForkPush (int num, char *args[])
{
    int nb_outs = atoi (args[1]);
    int i;

    INIT_COMP (BrzForkPush);
    INIT_COMP_PASSIVE_PORT (BrzForkPush, 0, atoi (args[2]));

    c->nb_outs = nb_outs;

    for (i = 0; i < nb_outs; i++)
    {
        INIT_COMP_ACTIVE_PORT (BrzForkPush, i + 1, atoi (args[i + 3]));
        INIT_COMP_CONTROLLED_DATA_PORT (i + 1, atoi (args[i + 3]));
    }

    WRITE_DEBUG_INFO_COMP (".Push", 1 + c->nb_outs);
}

/****************************************************/

struct BrzSynchPull_
{
    STRUCT_COMP_BASE;

    int nb_inputs;
    int nb_reqs;
    unsigned long long max_event_time;
};

void BrzSynchPull_requp (struct comp *comp, int port)
{
    struct BrzSynchPull_ *c = (struct BrzSynchPull_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs)
    {
        current_real_time = c->max_event_time;
        TRACE_FOLLOW_THREAD (c->nb_inputs, 0);
        SEND_REQUP (c->nb_inputs);
    }
}
void BrzSynchPull_ackup (struct comp *comp, int port)
{
    int i;
    struct BrzSynchPull_ *c = (struct BrzSynchPull_ *) comp;

    for (i = c->nb_inputs - 1; i >= 0; i--)
    {
        COPY_DATA_FROM_TO (c->nb_inputs, i);
        SEND_DATAON_DELAYED (i, 0);
        SEND_ACKUP_DELAYED (i, 1);
    }
    c->nb_reqs = 0;
    c->max_event_time = 0;
}

void BrzSynchPull_reqdown (struct comp *comp, int port)
{
    struct BrzSynchPull_ *c = (struct BrzSynchPull_ *) comp;

    c->nb_reqs++;
    c->max_event_time = MAX (c->max_event_time, current_real_time);
    if (c->nb_reqs == c->nb_inputs)
    {
        current_real_time = c->max_event_time;
        SEND_REQDOWN (c->nb_inputs);
    }
}
void BrzSynchPull_ackdown (struct comp *comp, int port)
{
    int i;
    struct BrzSynchPull_ *c = (struct BrzSynchPull_ *) comp;

    for (i = c->nb_inputs - 1; i >= 0; i--)
    {
        SEND_ACKDOWN_DELAYED (i, 0);
        SEND_DATAOFF_DELAYED (i, 1);
    }
    c->nb_reqs = 0;
    c->max_event_time = 0;
}

//void BrzSynchPull (int num, int width, int nb_inputs, ...) // n in, 1 out
void BrzSynchPull (int num, char *args[])
{
    int nb_inputs = atoi (args[1]);
    int i;

    INIT_COMP (BrzSynchPull);

    for (i = 0; i < nb_inputs; i++)
    {
        INIT_COMP_PASSIVE_PORT (BrzSynchPull, i, atoi (args[i + 2]));
        INIT_COMP_CONTROLLED_DATA_PORT (i, atoi (args[i + 2]));
    }

    INIT_COMP_ACTIVE_PORT (BrzSynchPull, nb_inputs, atoi (args[nb_inputs + 2]));

    c->nb_inputs = nb_inputs;
    c->nb_reqs = 0;
    c->max_event_time = 0;

    WRITE_DEBUG_INFO_COMP ("SynchPull", nb_inputs);
}

/****************************************************/
