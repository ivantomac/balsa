#include <gmodule.h>

//#define DEBUG_COSIM


static GModule *HLALibraryHandle = NULL;
int (*HLALibrary_initFunc) (int) = NULL;
int (*my_cosim__register_wire_in) (char *, char *, int) = NULL;
int (*my_cosim__register_wire_out) (char *, char *) = NULL;
int (*my_cosim__initialise) (int) = NULL;
int (*my_cosim__value_changed) (char *signal_name, char *signal_extension, char *signal_binstrvalue, int signal_time) = NULL;
int (*my_cosim__sync_callback) (int) = NULL;

typedef int (*HLA_FUNC) (int);


gpointer LoadHLALibrarySymbol (char *funcName)
{
    gpointer func;
    bool success;
    success = g_module_symbol (HLALibraryHandle, funcName + 1, (gpointer *) & func);

    if (!func || !success)
    {
        /* try BSD-style (?) _symbol */
        success = g_module_symbol (HLALibraryHandle, funcName, (gpointer *) & func);

        if (!func || !success)
        {
            g_error ("Can't find entry point %s in HLA library\n", funcName);
        }
    }

    return func;
}

void LoadHLALibrary (void)
{
    char *sharedLibraryFile = "./libmypli_breeze-sim.so";

    if (HLALibraryHandle)
        return;

    if (!g_module_supported ())
    {
        g_error ("GModule error: %s\n", g_module_error ());
    }

    HLALibraryHandle = g_module_open (sharedLibraryFile, G_MODULE_BIND_LAZY);

    if (!HLALibraryHandle)
    {
        g_error ("Can't find and load module %s\n", sharedLibraryFile);
        if (sharedLibraryFile)
            g_error ("GModule error2: %s\n", g_module_error ());
    } else
    {
        HLALibrary_initFunc =  LoadHLALibrarySymbol ("_initFunc");
        my_cosim__register_wire_in =  LoadHLALibrarySymbol ("_my__cosim__register_wire_in");
        my_cosim__register_wire_out =  LoadHLALibrarySymbol ("_my__cosim__register_wire_out");
        my_cosim__initialise =  LoadHLALibrarySymbol ("_my__cosim__initialise");
        my_cosim__value_changed =  LoadHLALibrarySymbol ("_my__cosim__value_changed");
        my_cosim__sync_callback =  LoadHLALibrarySymbol ("_my__cosim__sync_callback");
    }
}

char *convert_int_to_binary_string_with_length (unsigned int data, int length)
{
    char *str = g_strnfill (length, '0');
    int i;
    for (i=length-1; i>=0; i--)
    {
        if (data&1)
            str[i] = '1';
        data >>= 1;
    }
    return str;
}

/****************************************************/

struct BrzHLA_init_
{
    STRUCT_COMP_BASE;

};

void BrzHLA_init_requp (struct comp *comp, int port)
{
//    struct BrzHLA_init_ *c = (struct BrzHLA_init_ *) comp;

	printf ("BrzHLA_init_requp\n");
}
void BrzHLA_init_ackup (struct comp *comp, int port)
{
//    struct BrzHLA_init_ *c = (struct BrzHLA_init_ *) comp;

}
void BrzHLA_init_reqdown (struct comp *comp, int port)
{
//    struct BrzHLA_init_ *c = (struct BrzHLA_init_ *) comp;

}
void BrzHLA_init_ackdown (struct comp *comp, int port)
{
//    struct BrzHLA_init_ *c = (struct BrzHLA_init_ *) comp;

}

//void BrzHLA_init (int num, int activate)
void BrzHLA_init (int num, char *args[])
{
    INIT_COMP (BrzHLA_init);
    INIT_COMP_PASSIVE_PORT (BrzHLA_init, 0, atoi (args[1]));

    int id = atoi (args[0]);

    LoadHLALibrary ();
    HLALibrary_initFunc (0);
    my_cosim__initialise (id);

    WRITE_DEBUG_INFO_COMP ("HLA_init", 1);
}

/****************************************************/

struct BrzHLA_active_sync_
{
    STRUCT_COMP_BASE;

    char *channel_name;
};

void BrzHLA_active_sync_requp (struct comp *comp, int port)
{
//    struct BrzHLA_active_sync_ *c = (struct BrzHLA_active_sync_ *) comp;

    g_warning ("TODO: BrzHLA_active_sync_requp");
}
void BrzHLA_active_sync_ackup (struct comp *comp, int port)
{
    struct BrzHLA_active_sync_ *c = (struct BrzHLA_active_sync_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_sync_ackup");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_ack";
    char *binstrvalue = "1";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, binstrvalue, signal_time);
}
void BrzHLA_active_sync_reqdown (struct comp *comp, int port)
{
//    struct BrzHLA_active_sync_ *c = (struct BrzHLA_active_sync_ *) comp;

    g_warning ("TODO: BrzHLA_active_sync_reqdown");
}
void BrzHLA_active_sync_ackdown (struct comp *comp, int port)
{
    struct BrzHLA_active_sync_ *c = (struct BrzHLA_active_sync_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_sync_ackdown");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_ack";
    char *binstrvalue = "0";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, binstrvalue, signal_time);
}

//void BrzHLA_active_sync (int num, int activate)
void BrzHLA_active_sync (int num, char *args[])
{
    char *channel_name = args[0];
    int unique_port_req =  atoi (args[1]);

    if (channel_name[0]=='\"')
        channel_name++;

    INIT_COMP (BrzHLA_active_sync);
    INIT_COMP_ACTIVE_PORT (BrzHLA_active_sync, 0, atoi (args[1]));

    c->channel_name = g_strdup (channel_name);

    LoadHLALibrary ();
    my_cosim__register_wire_in (channel_name, "_req", unique_port_req);
    my_cosim__register_wire_out (channel_name, "_ack");

    WRITE_DEBUG_INFO_COMP ("HLA_active_sync", 1);
}

/****************************************************/

struct BrzHLA_active_push_
{
    STRUCT_COMP_BASE;

    char *channel_name;
    struct callback_ requp_cb, reqdown_cb;
};

void BrzHLA_active_push_requp (struct comp *comp, int port)
{
    struct BrzHLA_active_push_ *c = (struct BrzHLA_active_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_push_requp forwarding call");
#endif

    c->requp_cb.fct (c->requp_cb.comp, c->requp_cb.portnum);
}
void BrzHLA_active_push_ackup (struct comp *comp, int port)
{
    struct BrzHLA_active_push_ *c = (struct BrzHLA_active_push_ *) comp;
 
#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_push_ackup executing");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_ack";
    char *binstrvalue = "1";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, binstrvalue, signal_time);
}
void BrzHLA_active_push_ackdown (struct comp *comp, int port)
{
    struct BrzHLA_active_push_ *c = (struct BrzHLA_active_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_push_ackdown executing");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_ack";
    char *signal_binstrvalue = "0";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, signal_binstrvalue, signal_time);
}
void BrzHLA_active_push_reqdown (struct comp *comp, int port)
{
    struct BrzHLA_active_push_ *c = (struct BrzHLA_active_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_active_push_reqdown forwarding call");
#endif

    if (current_real_time < HLA_INITIALISATION_ENDING_TIME)  // real execution only starts at timestep HLA_INITIALISATION_ENDING_TIME. Before that is RTZ initialisation.
    {
#ifdef DEBUG_COSIM
        g_warning ("BrzHLA_active_push_reqdown transformed into initialisation ackdown");
#endif

        BrzHLA_active_push_ackdown (comp, port);
    }
    else
        c->reqdown_cb.fct (c->reqdown_cb.comp, c->reqdown_cb.portnum);
}

void BrzHLA_active_push_PostInitialisation (struct comp *comp, int optional_data)
{
    // Saving and replacing the requp/reqdown callback, in order to bypass the normal component (the one connected to this BrzHLA_active_push and process the signal before)
    // This is used to catch the initial reqdown sent by VCS initialisation

    struct BrzHLA_active_push_ *c = (struct BrzHLA_active_push_ *) comp;

    c->requp_cb = c->chan[0]->requp;
    c->reqdown_cb = c->chan[0]->reqdown;

    c->chan[0]->requp.fct = BrzHLA_active_push_requp;
    c->chan[0]->requp.comp = comp;
    c->chan[0]->requp.portnum = 0;
    c->chan[0]->reqdown.fct = BrzHLA_active_push_reqdown;
    c->chan[0]->reqdown.comp = comp;
    c->chan[0]->reqdown.portnum = 0;
}

//void BrzHLA_active_push (int num, int activate)
void BrzHLA_active_push (int num, char *args[])
{
    char *channel_name = args[0];
    int unique_port_req =  atoi (args[1]);

    if (channel_name[0]=='\"')
        channel_name++;

    INIT_COMP (BrzHLA_active_push);
    INIT_COMP_ACTIVE_PORT (BrzHLA_active_push, 0, atoi (args[1]));

    c->channel_name = g_strdup (channel_name);

    LoadHLALibrary ();
    my_cosim__register_wire_in (channel_name, "_datareq", unique_port_req);
    my_cosim__register_wire_out (channel_name, "_ack");

    AddComponentPostInitialisation (BrzHLA_active_push_PostInitialisation, c, 0);

    WRITE_DEBUG_INFO_COMP ("HLA_active_push", 1);
}

/****************************************************/

struct BrzHLA_passive_sync_
{
    STRUCT_COMP_BASE;

    char *channel_name;
};

void BrzHLA_passive_sync_requp (struct comp *comp, int port)
{
    struct BrzHLA_passive_sync_ *c = (struct BrzHLA_passive_sync_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_sync_requp");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_req";
    char *binstrvalue = "1";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, binstrvalue, signal_time);
}
void BrzHLA_passive_sync_ackup (struct comp *comp, int port)
{
//    struct BrzHLA_passive_sync_ *c = (struct BrzHLA_passive_sync_ *) comp;

//    my_cosim__value_changed (c->channel_name, "_ack", unique_port_ack, $time);
    g_warning ("TODO: BrzHLA_passive_sync_ackup");
}
void BrzHLA_passive_sync_reqdown (struct comp *comp, int port)
{
    struct BrzHLA_passive_sync_ *c = (struct BrzHLA_passive_sync_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_sync_reqdown");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_req";
    char *binstrvalue = "0";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, binstrvalue, signal_time);
}
void BrzHLA_passive_sync_ackdown (struct comp *comp, int port)
{
//    struct BrzHLA_passive_sync_ *c = (struct BrzHLA_passive_sync_ *) comp;

    g_warning ("TODO: BrzHLA_passive_sync_ackdown");
}

//void BrzHLA_passive_sync (int num, int activate)
void BrzHLA_passive_sync (int num, char *args[])
{
    char *channel_name = args[0];
    int unique_port_req =  atoi (args[1]);

    if (channel_name[0]=='\"')
        channel_name++;

    INIT_COMP (BrzHLA_passive_sync);
    INIT_COMP_PASSIVE_PORT (BrzHLA_passive_sync, 0, atoi (args[1]));

    c->channel_name = g_strdup (channel_name);

    LoadHLALibrary ();
    my_cosim__register_wire_out (channel_name, "_req");
    my_cosim__register_wire_in (channel_name, "_ack", unique_port_req);

    SEND_REQDOWN (0); // Port Setup for VCS simulation

    WRITE_DEBUG_INFO_COMP ("HLA_passive_sync", 1);
}

/****************************************************/

struct BrzHLA_passive_push_
{
    STRUCT_COMP_BASE;

    char *channel_name;
    int width;
    struct callback_ ackup_cb, ackdown_cb;
    gboolean first_time;
    gboolean ignore_ackdown;
};

void BrzHLA_passive_push_requp (struct comp *comp, int port)
{
    struct BrzHLA_passive_push_ *c = (struct BrzHLA_passive_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_push_requp executing");
#endif

    char *signal_name = c->channel_name;
    char *signal_extension = "_datareq";
    char *binstrvalue = "1";
    int signal_time = current_real_time;

    if (c->chan[0]->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
    {
        mpz_ptr data = (mpz_ptr) GET_PORT_DATA (0);
        binstrvalue = mpz_get_str (0, 2/*binary*/, data);
#ifdef DEBUG_COSIM
        printf ("converting (hexadecimal) value 0x");
        mpz_out_str (stdout, 16, data);
        printf ("\n");
#endif
    } else
    {
        long data = GET_PORT_DATA (0);
        binstrvalue = convert_int_to_binary_string_with_length (data, c->chan[0]->width); // g_strdup_printf ("%b", data);
#ifdef DEBUG_COSIM
        printf ("converting (binary) value %s\n", binstrvalue);
#endif
    }
    char *full_word = g_strnfill ( 2*c->chan[0]->width, '0');
    int i,j;
    for (i=c->chan[0]->width-strlen(binstrvalue), j=0; i<c->chan[0]->width; i++, j++) {
        full_word[i] = binstrvalue[j];
    }
    for (i=0; i<c->chan[0]->width; i++) {
        full_word[i+c->chan[0]->width] = '0'+(1-(full_word[i]-'0'));
    }

    my_cosim__value_changed (signal_name, signal_extension, full_word, signal_time);

//    g_free (binstrvalue);
}
void BrzHLA_passive_push_ackup (struct comp *comp, int port)
{
    struct BrzHLA_passive_push_ *c = (struct BrzHLA_passive_push_ *) comp;

//    my_cosim__value_changed (c->channel_name, "_ack", unique_port_ack, $time);
#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_push_ackup forwarding call");
#endif

    c->ackup_cb.fct (c->ackup_cb.comp, c->ackup_cb.portnum);
}
void BrzHLA_passive_push_ackdown (struct comp *comp, int port);
void BrzHLA_passive_push_reqdown (struct comp *comp, int port)
{
    struct BrzHLA_passive_push_ *c = (struct BrzHLA_passive_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_push_reqdown executing");
#endif

    if (c->first_time) {
        c->first_time = FALSE;

        c->ackup_cb = c->chan[0]->ackup;
        c->ackdown_cb = c->chan[0]->ackdown;

        c->chan[0]->ackup.fct = BrzHLA_passive_push_ackup;
        c->chan[0]->ackup.comp = comp;
        c->chan[0]->ackup.portnum = 0;
        c->chan[0]->ackdown.fct = BrzHLA_passive_push_ackdown;
        c->chan[0]->ackdown.comp = comp;
        c->chan[0]->ackdown.portnum = 0;

        c->ignore_ackdown = TRUE;
    }

    char *signal_name = c->channel_name;
    char *signal_extension = "_datareq";
    char *signal_binstrvalue = "00000000000000000000000000000000000000000000000000000000000000000000000000";
    int signal_time = current_real_time;

    my_cosim__value_changed (signal_name, signal_extension, signal_binstrvalue, signal_time);
}
void BrzHLA_passive_push_ackdown (struct comp *comp, int port)
{
    struct BrzHLA_passive_push_ *c = (struct BrzHLA_passive_push_ *) comp;

#ifdef DEBUG_COSIM
    g_warning ("BrzHLA_passive_push_ackdown forwarding call");
#endif

    if (c->ignore_ackdown)
    {
        c->ignore_ackdown = FALSE;
#ifdef DEBUG_COSIM
        g_warning ("BrzHLA_passive_push_ackdown ignored");
#endif
    }
    else
        c->ackdown_cb.fct (c->ackdown_cb.comp, c->ackdown_cb.portnum);
}

//void BrzHLA_passive_push (int num, int activate)
void BrzHLA_passive_push (int num, char *args[])
{
    char *channel_name = args[0];
    int unique_port_req =  atoi (args[1]);

    if (channel_name[0]=='\"')
        channel_name++;

    INIT_COMP (BrzHLA_passive_push);
    INIT_COMP_PASSIVE_PORT (BrzHLA_passive_push, 0, atoi (args[1]));

    c->channel_name = g_strdup (channel_name);
    c->first_time = TRUE;

    LoadHLALibrary ();
    my_cosim__register_wire_out (channel_name, "_datareq");
    my_cosim__register_wire_in (channel_name, "_ack", unique_port_req);

    SEND_REQDOWN (0); // Port Setup for VCS simulation

    WRITE_DEBUG_INFO_COMP ("HLA_passive_push", 1);
}


/****************************************************/

void breezesim_vpi_put_value (int channel_num, char *binstrval, char *name)
{
#ifdef DEBUG_COSIM
    printf ("breezesim_vpi_put_value %d %s %s\n", channel_num, binstrval, name);
#endif

    struct chan *chan = &channel[channel_num];

    int name_length = strlen (name);
    char *extension = name+name_length-3;
    struct callback_ *callback;
    mpz_ptr val;

    // check if the dual-rail binstrval == 0
    char *is_not_zero = binstrval;
    for (; *is_not_zero; is_not_zero++)
        if (*is_not_zero != '0')
            break;

    // check if the dual-rail value is valid
    if (*is_not_zero)
    {
#ifdef DEBUG_COSIM
        printf ("converting dual-rail value %s\n", binstrval);
#endif
        int length = strlen (binstrval)/2;
        int i;

        val = (mpz_ptr) malloc (sizeof (mpz_t));
        mpz_init (val);

        for (i=0; i<length; i++) {
            if (binstrval[i] != '0'+(1-(binstrval[i+length]-'0'))) {
#ifdef DEBUG_COSIM
                printf ("invalid dual-rail value (bit %d)\n", i);
#endif
                return;
            }
            mpz_mul_2exp (val, val, 1);
            if (binstrval[i]=='1')
                mpz_setbit (val, 0);
        }
#ifdef DEBUG_COSIM
        printf ("converted value = 0x");
        mpz_out_str (stdout, 16, val);
        printf ("\n");
#endif
    }

    if (!strcmp (extension, "req")) {
        if (strchr (binstrval, 'x')) {
#ifdef DEBUG_COSIM
            printf ("breezesim_vpi_put_value not forwarding value with \"x\"\n");
#endif
            return;
        }
        if (*is_not_zero)
            callback = &chan->requp;
        else
            callback = &chan->reqdown;
    }
    else if (!strcmp (extension, "ack")) {
        if (strchr (binstrval, 'x')) {
#ifdef DEBUG_COSIM
            printf ("breezesim_vpi_put_value not forwarding value with \"x\"\n");
#endif
            return;
        }
        if (*is_not_zero)
            callback = &chan->ackup;
        else
            callback = &chan->ackdown;
    }
    else
        g_error ("unknown extension (%s) for %s", extension, name);

    if (*is_not_zero) {
        struct comp *c = callback->comp;

        if (c->chan[0]->width)
        {
            if (c->chan[0]->width > (HOST_INTEGER_WIDTH_IN_BYTES * 8))
            {
                SET_PORT_DATA (0, (long) val);
            } else
            {
                unsigned int val2 = mpz_get_ui (val);

                SET_PORT_DATA (0, val2);
            }
        }
    }

    InsertEventAtLaterTime (callback, 0, 0);

//    free (val);
}
