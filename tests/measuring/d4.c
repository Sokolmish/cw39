int printf(const char *format, ...);
int atoi(const char *nptr);
int rand(void);
void *malloc(unsigned long size);
void free(void *ptr);
float logf(float x);
unsigned long strlen(const char *s);

typedef unsigned int u32;

void stop_dce(u32 *addr, u32 x) {
    volatile u32 *vaddr = addr;
    *vaddr = x;
}

void prefix(char *S, int *res) {
    res[0] = 0;
    int t = 0;
    for (int i = 1; i < strlen(S); ++i) {
        while (t > 0 && S[t] != S[i])
            t = res[t - 1];
        if (S[t] == S[i])
            ++t;
        res[i] = t;
    }
    return;
}

void KMPSubstr(char *S, char *T) {
    int *P = malloc(strlen(S));
    prefix(S, P);
    int q = 0;
    for (int k = 0; k < strlen(T); ++k) {
        while (q > 0 && S[q] != T[k])
            q = P[q - 1];
        if (S[q] == T[k])
            ++q;
        if (q == strlen(S)) {
//            printf("%i ", k - (int) strlen(S) + 1);
            u32 space;
            stop_dce(&space, k - (int) strlen(S) + 1);
        }
    }
    free((void*)P);
}

// FUT_D4
int FUT(int a1, int a2, int a3) {
    KMPSubstr("Ass323dadasdas21sda2134fdsfdsfcsadasdasdajasfbckkd408casnmcmnDSASsdasDSAD5   ASC   xDwa23vjnawiknc"
              "asewfdhfsadasdcjkansdjksdsxccczzvvnesdxdasdasxxda43dx8654Sjtscmmw3xcauj46 sdx a cxx asdasdajsadsds"
              "asewfdhfsadasdcjkansdjksdsdasdasdaaxm90u24kslndin12hn89nskdakjlhiksSscjk dasdc 2cjkx 2136tgfdvqcvx"
              "Ass323dadasdas21sddddasdasdksSsafsdascjkansdjksSssfdqwdq2dsad35yx3r53xd1 566366sd1xc qevdqcxqaascq"
              "SDASDQJ@#DSAMLKMKLSFSASAAAADSA214sxaAdjksSxcqw3csadfyg54hcmsklncqwcjqw[pi sJUlhjkds ciqwvubjb3uDoi"
              "XcjkansdjkssXcjkansdjksdasdacjkansdadksSszcjnkn38itnfnjufiqbgcjsb892bfjik; fhbc.1y iquyhjcxdi8hio3"
              "Ass323dadasdas21sddddasdasdksSsafsdaseiohgklcjo3in0o9pajwdpfn2j9cpsnjibriuw dfbfj bjksadajwdcsaNJA"
              "cjkansdjksSadASAWSsssdasfwraf32hdsvgfdasbKBAD@NSAIUsad2dqasyinjdkaSDfdaDssaf jka awdbiccDPZhcbaw53"
              "asdXXXcjkansdjksSsafdsdaasdsadasdsasfHDSDNKSDNCdsadaLDSVsfaN<LAFWLNs,daJLSfsa   cjSKcasbwakjFASchb"
              "ASDSAD42sdjasdklj21ikjedKML&75463dFFFSASHADBSACMBNBasdBCAasdfsaDGsjhds153daKWX CdsaASBJKDAWJDsadfa",
              "cjkansdjksSsaf");
    return 0;
}
