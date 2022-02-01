/*
 *  LibXDiff by Davide Libenzi ( File Differential Library )
 *  Copyright (C) 2003  Davide Libenzi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(WIN32) || defined(_WIN32)

#include <io.h>

#define write _write
#define read _read
#define lseek _lseek
#define close _close
#define O_CREAT _O_CREAT
#define O_WRONLY _O_WRONLY
#define O_RDONLY _O_RDONLY
#define off_t long
#define size_t int

#else /* #if defined(WIN32) || defined(_WIN32) */

#include <unistd.h>

#endif /* #if defined(WIN32) || defined(_WIN32) */

#include "xmacros.h"
#include "xdiff.h"
#include "xtestutils.h"



#define XDLT_STD_BLKSIZE (1024 * 8)
#define XDLT_MAX_LINE_SIZE 80




//static int xdlt_mmfile_outf(void *priv, mmbuffer_t *mb, int nbuf);





//int xdlt_dump_mmfile(char const *fname, mmfile_t *mmf) {
//	int fd;
//	long size;
//	char *blk;
//
//	if ((fd = open(fname, O_CREAT | O_WRONLY, 0644)) == -1) {
//		perror(fname);
//		return -1;
//	}
//
//	if ((blk = (char *) xdl_mmfile_first(mmf, &size)) != NULL) {
//		do {
//			if (write(fd, blk, (size_t) size) != (size_t) size) {
//				perror(fname);
//				close(fd);
//				return -1;
//			}
//		} while ((blk = (char *) xdl_mmfile_next(mmf, &size)) != NULL);
//	}
//
//	close(fd);
//
//	return 0;
//}

//int xdlt_load_mmfile(char const *fname, mmfile_t *mf, int binmode) {
//	char cc;
//	int fd;
//	long size;
//	char *blk;
//
//	if (xdl_init_mmfile(mf, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	if ((fd = open(fname, O_RDONLY)) == -1) {
//		perror(fname);
//		xdl_free_mmfile(mf);
//		return -1;
//	}
//	size = lseek(fd, 0, SEEK_END);
//	lseek(fd, 0, SEEK_SET);
//	if (!(blk = (char *)xdl_mmfile_writeallocate(mf, size))) {
//		xdl_free_mmfile(mf);
//		close(fd);
//		return -1;
//	}
//	int bytesRead = read(fd, blk, (size_t)size);
//	if (bytesRead != (size_t)size) {
//		perror(fname);
//		xdl_free_mmfile(mf);
//		close(fd);
//		return -1;
//	}
//	close(fd);
//
//	return 0;
//}

int xdlt_load_mmfile(char const *fname, mmfile_t *mf, int binmode) {
	char cc;
	//int fd;
	long size;
	char *blk;

	if (xdl_init_mmfile(mf, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {

		return -1;
	}

	FILE *fp = fopen(fname, "rb");
	if (fp == NULL)
	{
		xdl_free_mmfile(mf);
		fclose(fp);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	if (!(blk = (char *) xdl_mmfile_writeallocate(mf, size))) {
		xdl_free_mmfile(mf);
		fclose(fp);
		return -1;
	}

	int count = fread(blk, size, 1, fp);
	if (count != 1) {
		perror(fname);
		xdl_free_mmfile(mf);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return 0;
}


static int xdlt_mmfile_outf(void *priv, mmbuffer_t *mb, int nbuf) {
	mmfile_t *mmf = priv;

	if (xdl_writem_mmfile(mmf, mb, nbuf) < 0) {

		return -1;
	}

	return 0;
}


//int xdlt_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
//		 xdemitconf_t const *xecfg, mmfile_t *mfp) {
//	xdemitcb_t ecb;
//
//	if (xdl_init_mmfile(mfp, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	ecb.priv = mfp;
//	ecb.outf = xdlt_mmfile_outf;
//	if (xdl_diff(mf1, mf2, xpp, xecfg, &ecb) < 0) {
//
//		xdl_free_mmfile(mfp);
//		return -1;
//	}
//
//	return 0;
//}


//int xdlt_do_patch(mmfile_t *mfo, mmfile_t *mfp, int mode, mmfile_t *mfr) {
//	xdemitcb_t ecb, rjecb;
//	mmfile_t mmfrj;
//
//	if (xdl_init_mmfile(mfr, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	if (xdl_init_mmfile(&mmfrj, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		xdl_free_mmfile(mfr);
//		return -1;
//	}
//	ecb.priv = mfr;
//	ecb.outf = xdlt_mmfile_outf;
//	rjecb.priv = &mmfrj;
//	rjecb.outf = xdlt_mmfile_outf;
//	if (xdl_patch(mfo, mfp, mode, &ecb, &rjecb) < 0) {
//
//		xdl_free_mmfile(&mmfrj);
//		xdl_free_mmfile(mfr);
//		return -1;
//	}
//
//	if (mmfrj.fsize > 0) {
//
//#if 1
//		xdlt_dump_mmfile("xregr.orig", mfo);
//		xdlt_dump_mmfile("xregr.patch", mfp);
//		xdlt_dump_mmfile("xregr.rej", &mmfrj);
//#endif
//
//		xdl_free_mmfile(&mmfrj);
//		xdl_free_mmfile(mfr);
//		return -1;
//	}
//	xdl_free_mmfile(&mmfrj);
//
//	return 0;
//}


//int xdlt_do_regress(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
//		    xdemitconf_t const *xecfg) {
//	mmfile_t mfp, mfr;
//
//	if (xdlt_do_diff(mf1, mf2, xpp, xecfg, &mfp) < 0) {
//
//		return -1;
//	}
//	if (xdlt_do_patch(mf1, &mfp, XDL_PATCH_NORMAL, &mfr) < 0) {
//
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	if (xdl_mmfile_cmp(&mfr, mf2)) {
//
//		xdl_free_mmfile(&mfr);
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	xdl_free_mmfile(&mfr);
//	if (xdlt_do_patch(mf2, &mfp, XDL_PATCH_REVERSE, &mfr) < 0) {
//
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	if (xdl_mmfile_cmp(&mfr, mf1)) {
//
//		xdl_free_mmfile(&mfr);
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	xdl_free_mmfile(&mfr);
//	xdl_free_mmfile(&mfp);
//
//	return 0;
//}


//long xdlt_gen_line(char *buf, long msize) {
//	long i, size;
//	static const char ab[] =
//		"zxcvbnmlkjhgfdsaqwertyuiop"
//		"ZXCVBNMLKJHGFDSAQWERTYUIOP"
//		"0123456789                ";
//	static const int absize = sizeof(ab) - 1;
//
//	if (msize > 0)
//		size = rand() % (msize - 1);
//	else
//		size = -msize - 1;
//	for (i = 0; i < size; i++)
//		buf[i] = ab[rand() % absize];
//	buf[i] = '\n';
//	return size + 1;
//}


//int xdlt_create_file(mmfile_t *mf, long size) {
//	long lnsize, csize;
//	char *data;
//
//	if (xdl_init_mmfile(mf, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	if (!(data = xdl_mmfile_writeallocate(mf, size))) {
//
//		xdl_free_mmfile(mf);
//		return -1;
//	}
//	for (csize = 0; size - csize > XDLT_MAX_LINE_SIZE;) {
//		lnsize = xdlt_gen_line(data, XDLT_MAX_LINE_SIZE);
//		data += lnsize;
//		csize += lnsize;
//	}
//	if (csize < size)
//		xdlt_gen_line(data, -(size - csize));
//
//	return 0;
//}


//int xdlt_change_file(mmfile_t *mfo, mmfile_t *mfr, double rmod,
//		     int chmax) {
//	long skipln, lnsize, bsize;
//	char const *blk, *cur, *top, *eol;
//	char lnbuf[XDLT_MAX_LINE_SIZE + 1];
//
//	if (xdl_init_mmfile(mfr, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	if ((blk = xdl_mmfile_first(mfo, &bsize)) != NULL) {
//		for (cur = blk, top = blk + bsize, skipln = 0;;) {
//			if (cur >= top) {
//				if ((blk = xdl_mmfile_next(mfo, &bsize)) == NULL)
//					break;
//				cur = blk;
//				top = blk + bsize;
//			}
//			if (!(eol = memchr(cur, '\n', top - cur)))
//				eol = top;
//			if (!skipln) {
//				if (DBL_RAND() < rmod) {
//					skipln = rand() % chmax;
//					if (rand() & 1) {
//						for (; skipln > 0; skipln--) {
//							lnsize = xdlt_gen_line(lnbuf, XDLT_MAX_LINE_SIZE);
//							if (xdl_write_mmfile(mfr, lnbuf, lnsize) != lnsize) {
//
//								xdl_free_mmfile(mfr);
//								return -1;
//							}
//						}
//					}
//				} else {
//					lnsize = (eol - cur) + 1;
//					if (xdl_write_mmfile(mfr, cur, lnsize) != lnsize) {
//
//						xdl_free_mmfile(mfr);
//						return -1;
//					}
//				}
//			} else
//				skipln--;
//
//			cur = eol + 1;
//		}
//	}
//
//	return 0;
//}


//int xdlt_auto_regress(xpparam_t const *xpp, xdemitconf_t const *xecfg, long size,
//		      double rmod, int chmax) {
//	mmfile_t mf1, mf2;
//
//	if (xdlt_create_file(&mf1, size) < 0) {
//
//		return -1;
//	}
//	if (xdlt_change_file(&mf1, &mf2, rmod, chmax) < 0) {
//
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	if (xdlt_do_regress(&mf1, &mf2, xpp, xecfg) < 0) {
//
//		xdl_free_mmfile(&mf2);
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	xdl_free_mmfile(&mf2);
//	xdl_free_mmfile(&mf1);
//
//	return 0;
//}


//int xdlt_do_bindiff(mmfile_t *mf1, mmfile_t *mf2, bdiffparam_t const *bdp, mmfile_t *mfp) {
//	xdemitcb_t ecb;
//
//	if (xdl_init_mmfile(mfp, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	ecb.priv = mfp;
//	ecb.outf = xdlt_mmfile_outf;
//	if (xdl_bdiff(mf1, mf2, bdp, &ecb) < 0) {
//
//		xdl_free_mmfile(mfp);
//		return -1;
//	}
//
//	return 0;
//}


//int xdlt_do_rabdiff(mmfile_t *mf1, mmfile_t *mf2, mmfile_t *mfp) {
//	xdemitcb_t ecb;
//
//	if (xdl_init_mmfile(mfp, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	ecb.priv = mfp;
//	ecb.outf = xdlt_mmfile_outf;
//	if (xdl_rabdiff(mf1, mf2, &ecb) < 0) {
//
//		xdl_free_mmfile(mfp);
//		return -1;
//	}
//
//	return 0;
//}


//int xdlt_do_binpatch(mmfile_t *mf, mmfile_t *mfp, mmfile_t *mfr) {
//	xdemitcb_t ecb;
//
//	if (xdl_init_mmfile(mfr, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		return -1;
//	}
//	ecb.priv = mfr;
//	ecb.outf = xdlt_mmfile_outf;
//	if (xdl_bpatch(mf, mfp, &ecb) < 0) {
//
//		xdl_free_mmfile(mfr);
//		return -1;
//	}
//
//	return 0;
//}


//int xdlt_do_binregress(mmfile_t *mf1, mmfile_t *mf2, bdiffparam_t const *bdp) {
//	mmfile_t mfp, mfr;
//
//	if (xdlt_do_bindiff(mf1, mf2, bdp, &mfp) < 0) {
//
//		return -1;
//	}
//	if (xdlt_do_binpatch(mf1, &mfp, &mfr) < 0) {
//
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	if (xdl_mmfile_cmp(&mfr, mf2)) {
//
//		xdl_free_mmfile(&mfr);
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	xdl_free_mmfile(&mfr);
//	xdl_free_mmfile(&mfp);
//
//	return 0;
//}


//int xdlt_do_rabinregress(mmfile_t *mf1, mmfile_t *mf2) {
//	mmfile_t mfp, mfr;
//
//	if (xdlt_do_rabdiff(mf1, mf2, &mfp) < 0) {
//
//		return -1;
//	}
//	if (xdlt_do_binpatch(mf1, &mfp, &mfr) < 0) {
//
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	if (xdl_mmfile_cmp(&mfr, mf2)) {
//
//		xdl_free_mmfile(&mfr);
//		xdl_free_mmfile(&mfp);
//		return -1;
//	}
//	xdl_free_mmfile(&mfr);
//	xdl_free_mmfile(&mfp);
//
//	return 0;
//}


//int xdlt_auto_binregress(bdiffparam_t const *bdp, long size,
//			 double rmod, int chmax) {
//	mmfile_t mf1, mf2, mf2c;
//
//	if (xdlt_create_file(&mf1, size) < 0) {
//
//		return -1;
//	}
//	if (xdlt_change_file(&mf1, &mf2, rmod, chmax) < 0) {
//
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	if (xdl_mmfile_compact(&mf2, &mf2c, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		xdl_free_mmfile(&mf2);
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	xdl_free_mmfile(&mf2);
//	if (xdlt_do_binregress(&mf1, &mf2c, bdp) < 0) {
//
//		xdl_free_mmfile(&mf2c);
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	xdl_free_mmfile(&mf2c);
//	xdl_free_mmfile(&mf1);
//
//	return 0;
//}


//int xdlt_auto_rabinregress(long size, double rmod, int chmax) {
//	mmfile_t mf1, mf2, mf2c;
//
//	if (xdlt_create_file(&mf1, size) < 0) {
//
//		return -1;
//	}
//	if (xdlt_change_file(&mf1, &mf2, rmod, chmax) < 0) {
//
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	if (xdl_mmfile_compact(&mf2, &mf2c, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		xdl_free_mmfile(&mf2);
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	xdl_free_mmfile(&mf2);
//	if (xdlt_do_rabinregress(&mf1, &mf2c) < 0) {
//
//		xdl_free_mmfile(&mf2c);
//		xdl_free_mmfile(&mf1);
//		return -1;
//	}
//	xdl_free_mmfile(&mf2c);
//	xdl_free_mmfile(&mf1);
//
//	return 0;
//}


//int xdlt_auto_mbinregress(bdiffparam_t const *bdp, long size,
//			  double rmod, int chmax, int n) {
//	int i, res;
//	mmbuffer_t *mbb;
//	mmfile_t *mf, *mfc, *mfx;
//	mmfile_t mfn, mff, mfd, mfb;
//	xdemitcb_t ecb;
//
//	if ((mbb = (mmbuffer_t *) xdl_malloc((n + 2) * sizeof(mmbuffer_t))) == NULL) {
//
//		return -1;
//	}
//	if ((mf = mfc = (mmfile_t *) xdl_malloc((n + 2) * sizeof(mmfile_t))) == NULL) {
//
//		xdl_free(mbb);
//		return -1;
//	}
//	if (xdlt_create_file(mfc, size) < 0) {
//
//		xdl_free(mf);
//		xdl_free(mbb);
//		return -1;
//	}
//	mbb[0].ptr = (char *) xdl_mmfile_first(mfc, &mbb[0].size);
//	mfc++;
//	mfx = mf;
//	for (i = 0; i < n; i++) {
//		if (xdlt_change_file(mfx, &mfn, rmod, chmax) < 0) {
//
//			if (mfx != mf) xdl_free_mmfile(mfx);
//			for (; i >= 0; i--)
//				xdl_free_mmfile(mf + i);
//			xdl_free(mf);
//			xdl_free(mbb);
//			return -1;
//		}
//		if (xdl_mmfile_compact(&mfn, &mff, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//			xdl_free_mmfile(&mfn);
//			if (mfx != mf) xdl_free_mmfile(mfx);
//			for (; i >= 0; i--)
//				xdl_free_mmfile(mf + i);
//			xdl_free(mf);
//			xdl_free(mbb);
//			return -1;
//		}
//		xdl_free_mmfile(&mfn);
//		if (xdlt_do_bindiff(mfx, &mff, bdp, &mfd) < 0) {
//
//			xdl_free_mmfile(&mff);
//			if (mfx != mf) xdl_free_mmfile(mfx);
//			for (; i >= 0; i--)
//				xdl_free_mmfile(mf + i);
//			xdl_free(mf);
//			xdl_free(mbb);
//			return -1;
//		}
//		if (mfx != mf) xdl_free_mmfile(mfx);
//		mfx = &mfb;
//		*mfx = mff;
//		if (xdl_mmfile_compact(&mfd, mfc, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//			xdl_free_mmfile(&mfd);
//			xdl_free_mmfile(mfx);
//			for (; i >= 0; i--)
//				xdl_free_mmfile(mf + i);
//			xdl_free(mf);
//			xdl_free(mbb);
//			return -1;
//		}
//		mbb[i + 1].ptr = (char *) xdl_mmfile_first(mfc, &mbb[i + 1].size);
//		mfc++;
//		xdl_free_mmfile(&mfd);
//	}
//	if (xdl_init_mmfile(mfc, XDLT_STD_BLKSIZE, XDL_MMF_ATOMIC) < 0) {
//
//		xdl_free_mmfile(mfx);
//		for (i = n; i >= 0; i--)
//			xdl_free_mmfile(mf + i);
//		xdl_free(mf);
//		xdl_free(mbb);
//		return -1;
//	}
//	ecb.priv = mfc;
//	ecb.outf = xdlt_mmfile_outf;
//	if ((res = xdl_bpatch_multi(&mbb[0], &mbb[1], n, &ecb)) == 0)
//		res = xdl_mmfile_cmp(mfx, mfc);
//
//	xdl_free_mmfile(mfx);
//	for (i = n + 1; i >= 0; i--)
//		xdl_free_mmfile(mf + i);
//	xdl_free(mf);
//	xdl_free(mbb);
//
//	return res;
//}

