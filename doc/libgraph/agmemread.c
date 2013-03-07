typedef struct {
    const char *data;
    int len;
    int cur;
} rdr_t;

static int memiofread(void *chan, char *buf, int bufsize)
{
    const char *ptr;
    char *optr;
    char c;
    int l;
    rdr_t *s;

    if (bufsize == 0) return 0;
    s = (rdr_t *) chan;
    if (s->cur >= s->len)
        return 0;
    l = 0;
    ptr = s->data + s->cur;
    optr = buf;
    do {
        *optr++ = c = *ptr++;
        l++;
    } while (c && (c != '\n') && (l < bufsize));
    s->cur += l;
    return l;
}

static Agiodisc_t memIoDisc = {memiofread, 0, 0};

Agraph_t *agmemread(const char *cp)
{
    rdr_t rdr;
    Agdisc_t disc;
    Agiodisc_t memIoDisc;

    memIoDisc.putstr = AgIoDisc.putstr;
    memIoDisc.flush = AgIoDisc.flush;
    rdr.data = cp;
    rdr.len = strlen(cp);
    rdr.cur = 0;

    disc.mem = NULL;
    disc.id = NULL;
    disc.io = &memIoDisc;
    return agread (&rdr, &disc);
}
