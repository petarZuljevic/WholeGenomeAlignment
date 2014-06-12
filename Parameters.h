#ifndef _PARAMETERS_H
#define _PARAMETERS_H 1

//Broj iteracija pri traženju MUM-ova
int num_of_iter = 2;

//Minimalne duljine MUM-ova za svaku iteraciju
int min_len_fwd[] = {20, 20, 70};

//Minimalne duljine REVERZNIH MUM-ova za svaku iteraciju
int min_len_rev[] = {20, 20, 90};

//Najmanja udaljenost koja mora biti između para MUMova da bi se tražili novi na području između njih
int min_width = 500;

#endif /* _PARAMETERS_H */
