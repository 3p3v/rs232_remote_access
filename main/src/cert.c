#include <cert.h>
#include <string.h>
#include <memory.h>

#define NVS_CHAIN_SIZE "NVS_CHAIN_SIZE" // How many chaines are stored in nvs
#define NVS_CHAIN_SEGMENT_PREFIX "NVS_CHAIN_SEGMENT_" // prefix

char **cert_load_chain()
{
    char ***chain = cert_get_chain();
    // TODO get from nvs
    const unsigned char *ca_chain[] = {(const unsigned char *)SERVER_IM2_CERT,
                                       (const unsigned char *)SERVER_IM1_CERT,
                                       (const unsigned char *)SERVER_ROOT_CERT};//,
                                    //    NULL};
    *chain = realloc(*chain, sizeof(ca_chain) / sizeof(unsigned char *));

    for (int i = 0; i < (sizeof(ca_chain) / sizeof(unsigned char *)); i++)
    {
        

        *(*chain + i) = malloc(sizeof(unsigned char) * (strlen((*((char **)ca_chain + i))) + 1));
        memcpy(*(*chain + i), *(((char **)ca_chain) + i), strlen(*(((char **)ca_chain) + i)) + 1);
    }

    *(*chain + (sizeof(ca_chain) / sizeof(unsigned char *) - 1)) = NULL;

    return *chain;
}

char ***cert_get_chain()
{
    static char **chain = NULL;
    return &chain;
}

char **cert_set_save_chain(const char ** new_chain)
{
    char ***chain = cert_get_chain();
    //TODO save to nvs
    // chain = realloc(password, sizeof(char) * (strlen(new_password) + 1));
    return *chain;
}

char **cert_free_chain()
{
    char ***chain = cert_get_chain();
    unsigned int i = 0;
    for(;;)
    {
        if (*(*chain + i) == NULL)
            break;

        free(*(*chain + i));

        i++;
    }
    free(*chain);
    return *chain;
}