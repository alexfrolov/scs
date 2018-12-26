#include "rw.h"
#include "scs.h"
#include "util.h"
#include "amatrix.h"
#include <stdio.h>
#include <stdlib.h>

/* writes/reads problem data to/from filename */
/* This is a VERY naive implementation, doesn't care about portability etc */

static void write_scs_cone(const ScsCone *k, FILE *fout) {
  fwrite(&(k->f), sizeof(scs_int), 1, fout);
  fwrite(&(k->l), sizeof(scs_int), 1, fout);
  fwrite(&(k->qsize), sizeof(scs_int), 1, fout);
  fwrite(k->q, sizeof(scs_int), k->qsize, fout);
  fwrite(&(k->ssize), sizeof(scs_int), 1, fout);
  fwrite(k->s, sizeof(scs_int), k->ssize, fout);
  fwrite(&(k->ep), sizeof(scs_int), 1, fout);
  fwrite(&(k->ed), sizeof(scs_int), 1, fout);
  fwrite(&(k->psize), sizeof(scs_int), 1, fout);
  fwrite(k->p, sizeof(scs_float), k->psize, fout);
}

static ScsCone * read_scs_cone(FILE *fin) {
  ScsCone * k = (ScsCone *)scs_calloc(1, sizeof(ScsCone));
  fread(&(k->f), sizeof(scs_int), 1, fin);
  fread(&(k->l), sizeof(scs_int), 1, fin);
  fread(&(k->qsize), sizeof(scs_int), 1, fin);
  k->q = scs_calloc(k->qsize, sizeof(scs_int));
  fread(k->q, sizeof(scs_int), k->qsize, fin);
  fread(&(k->ssize), sizeof(scs_int), 1, fin);
  k->s = scs_calloc(k->ssize, sizeof(scs_int));
  fread(k->s, sizeof(scs_int), k->ssize, fin);
  fread(&(k->ep), sizeof(scs_int), 1, fin);
  fread(&(k->ed), sizeof(scs_int), 1, fin);
  fread(&(k->psize), sizeof(scs_int), 1, fin);
  k->p = scs_calloc(k->psize, sizeof(scs_int));
  fread(k->p, sizeof(scs_float), k->psize, fin);
  RETURN k;
}

static void write_scs_stgs(const ScsSettings *s, FILE *fout) {
  /* Warm start to false for now */
  scs_int warm_start = 0;
  fwrite(&(s->normalize), sizeof(scs_int), 1, fout);
  fwrite(&(s->scale), sizeof(scs_float), 1, fout);
  fwrite(&(s->rho_x), sizeof(scs_float), 1, fout);
  fwrite(&(s->max_iters), sizeof(scs_int), 1, fout);
  fwrite(&(s->eps), sizeof(scs_float), 1, fout);
  fwrite(&(s->alpha), sizeof(scs_float), 1, fout);
  fwrite(&(s->cg_rate), sizeof(scs_float), 1, fout);
  fwrite(&(s->verbose), sizeof(scs_int), 1, fout);
  fwrite(&warm_start, sizeof(scs_int), 1, fout);
  fwrite(&(s->acceleration_lookback), sizeof(scs_int), 1, fout);
  /* Do not write the data_write_filename */
}

static ScsSettings * read_scs_stgs(FILE *fin) {
  ScsSettings * s = (ScsSettings *)scs_calloc(1, sizeof(ScsSettings));
  fread(&(s->normalize), sizeof(scs_int), 1, fin);
  fread(&(s->scale), sizeof(scs_float), 1, fin);
  fread(&(s->rho_x), sizeof(scs_float), 1, fin);
  fread(&(s->max_iters), sizeof(scs_int), 1, fin);
  fread(&(s->eps), sizeof(scs_float), 1, fin);
  fread(&(s->alpha), sizeof(scs_float), 1, fin);
  fread(&(s->cg_rate), sizeof(scs_float), 1, fin);
  fread(&(s->verbose), sizeof(scs_int), 1, fin);
  fread(&(s->warm_start), sizeof(scs_int), 1, fin);
  fread(&(s->acceleration_lookback), sizeof(scs_int), 1, fin);
  RETURN s;
}

static void write_amatrix(const ScsMatrix * A, FILE * fout) {
  scs_int Anz = A->p[A->n];
  fwrite(&(A->m), sizeof(scs_int), 1, fout);
  fwrite(&(A->n), sizeof(scs_int), 1, fout);
  fwrite(A->p, sizeof(scs_int), A->n + 1, fout);
  fwrite(A->x, sizeof(scs_float), Anz, fout);
  fwrite(A->i, sizeof(scs_int), Anz, fout);
}

static ScsMatrix * read_amatrix(FILE * fin) {
  scs_int Anz;
  ScsMatrix * A = (ScsMatrix *)scs_calloc(1, sizeof(ScsMatrix));
  fread(&(A->m), sizeof(scs_int), 1, fin);
  fread(&(A->n), sizeof(scs_int), 1, fin);
  A->p = scs_calloc(A->n + 1, sizeof(scs_int));
  fread(A->p, sizeof(scs_int), A->n + 1, fin);
  Anz = A->p[A->n];
  A->x = scs_calloc(Anz, sizeof(scs_float));
  A->i = scs_calloc(Anz, sizeof(scs_int));
  fread(A->x, sizeof(scs_float), Anz, fin);
  fread(A->i, sizeof(scs_int), Anz, fin);
  RETURN A;
}

static void write_scs_data(const ScsData *d, FILE *fout) {
  fwrite(&(d->m), sizeof(scs_int), 1, fout);
  fwrite(&(d->n), sizeof(scs_int), 1, fout);
  fwrite(d->b, sizeof(scs_float), d->m, fout);
  fwrite(d->c, sizeof(scs_float), d->n, fout);
  write_scs_stgs(d->stgs, fout);
  write_amatrix(d->A, fout);
}

static ScsData * read_scs_data(FILE *fin) {
  ScsData * d = (ScsData *)scs_calloc(1, sizeof(ScsData));
  fread(&(d->m), sizeof(scs_int), 1, fin);
  fread(&(d->n), sizeof(scs_int), 1, fin);
  d->b = scs_calloc(d->m, sizeof(scs_float));
  d->c = scs_calloc(d->n, sizeof(scs_float));
  fread(d->b, sizeof(scs_float), d->m, fin);
  fread(d->c, sizeof(scs_float), d->n, fin);
  RETURN d;
}


void SCS(read_data)(const char * filename, ScsData ** d, ScsCone ** k) {
  FILE* fin = fopen(filename, "rb");
  scs_printf("fin %i\n", fin);
  scs_printf("reading data from %s\n", filename);
  *k = read_scs_cone(fin);
  *d = read_scs_data(fin);
  fclose(fin);
}

void SCS(write_data)(const ScsData *d, const ScsCone *k) {
  FILE* fout = fopen(d->stgs->write_data_filename, "wb");
  scs_printf("writing data to %s\n", d->stgs->write_data_filename);
  write_scs_cone(k, fout);
  write_scs_data(d, fout);
  fclose(fout);
}
