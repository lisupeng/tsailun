#include "xdiff_interface.h"
#include "xdiff.h"
#include "xtestutils.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static memallocator_t malt;

static void *wrap_malloc(void *priv, unsigned int size) {

	return malloc(size);
}


static void wrap_free(void *priv, void *ptr) {

	free(ptr);
}


static void *wrap_realloc(void *priv, void *ptr, unsigned int size) {

	return realloc(ptr, size);
}

static int xdlt_outf(void *priv, mmbuffer_t *mb, int nbuf) {
	int i;

	for (i = 0; i < nbuf; i++)
		if (!fwrite(mb[i].ptr, mb[i].size, 1, (FILE *)priv))
			return -1;

	return 0;
}

void libxdiff_init()
{
	malt.priv = NULL;
	malt.malloc = wrap_malloc;
	malt.free = wrap_free;
	malt.realloc = wrap_realloc;
	xdl_set_allocator(&malt);
}

int libxdiff_diff(const char *oldfile, const char *newfile, const char *patchfile)
{
	mmfile_t mf1, mf2;
	xpparam_t xpp;
	xdemitconf_t xecfg;
	int ctxlen = 3;
	int bsize = 16;
	bdiffparam_t bdp;
	xdemitcb_t ecb;
	FILE *fpout;

	xpp.flags = 0;
	xecfg.ctxlen = ctxlen;
	bdp.bsize = bsize;
	if (xdlt_load_mmfile(oldfile, &mf1, 0) < 0) {

		return -1;
	}
	if (xdlt_load_mmfile(newfile, &mf2, 0) < 0) {

		xdl_free_mmfile(&mf1);
		return -1;
	}

	fpout = fopen(patchfile, "wb");
	if (fpout == NULL)
	{
		xdl_free_mmfile(&mf2);
		xdl_free_mmfile(&mf1);
	}

	ecb.priv = fpout;
	ecb.outf = xdlt_outf;

	if (xdl_diff(&mf1, &mf2, &xpp, &xecfg, &ecb) < 0) {

		xdl_free_mmfile(&mf2);
		xdl_free_mmfile(&mf1);
		fclose(fpout);
		return -1;
	}

	xdl_free_mmfile(&mf2);
	xdl_free_mmfile(&mf1);
	fclose(fpout);

	return 0;
}

int libxdiff_patch(const char *oldfile, const char *patchfile, const char *newfile, const char *rejfile)
{
	mmfile_t mf1, mf2;
	xdemitcb_t ecb, rjecb;
	FILE *fp1;
	FILE *fp2;

	if (xdlt_load_mmfile(oldfile, &mf1, 0) < 0) {

		return -1;
	}
	if (xdlt_load_mmfile(patchfile, &mf2, 0) < 0) {

		xdl_free_mmfile(&mf1);
		return -1;
	}

	fp1 = fopen(newfile, "wb");
	if (fp1 == NULL)
	{
		xdl_free_mmfile(&mf2);
		xdl_free_mmfile(&mf1);
		return -1;
	}

	fp2 = fopen(rejfile, "wb");
	if (fp2 == NULL)
	{
		xdl_free_mmfile(&mf2);
		xdl_free_mmfile(&mf1);
		fclose(fp1);
		return -1;
	}

	ecb.priv = fp1;
	ecb.outf = xdlt_outf;
	rjecb.priv = fp2;
	rjecb.outf = xdlt_outf;
	if (xdl_patch(&mf1, &mf2, XDL_PATCH_NORMAL, &ecb, &rjecb) < 0)
	{
		xdl_free_mmfile(&mf2);
		xdl_free_mmfile(&mf1);
		fclose(fp1);
		fclose(fp2);
		return -1;
    }

	xdl_free_mmfile(&mf2);
	xdl_free_mmfile(&mf1);
	fclose(fp1);
	fclose(fp2);
	return 0;
}

#ifdef __cplusplus
}
#endif