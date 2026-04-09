#include <stdio.h>
#include "vuln.h"

void print_vuln(vuln_t v)
{
    int i;

    printf("===== Vulnerability Scan Result =====\n");
    printf("has_gets        : %d\n", v.has_gets);
    printf("has_strcpy      : %d\n", v.has_strcpy);
    printf("has_rwx_segment : %d\n", v.has_rwx_segment);
    printf("message count   : %d\n", v.count);

    for (i = 0; i < v.count; i++)
    {
        if (v.messages[i])
            printf("[%d] %s\n", i + 1, v.messages[i]);
    }
}

void print_mitigation(mitigation_t m)
{
    printf("NX: %s\n", m.nx ? "Enabled" : "Disabled");
    printf("PIE: %s\n", m.pie ? "Enabled" : "Disabled");

    if (m.relro == 0)
        printf("RELRO: No RELRO\n");
    else if (m.relro == 1)
        printf("RELRO: Partial RELRO\n");
    else
        printf("RELRO: Full RELRO\n");

    printf("Canary: %s\n", m.canary ? "Enabled" : "Disabled");
}
