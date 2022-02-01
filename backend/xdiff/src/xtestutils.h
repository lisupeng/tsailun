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

#if !defined(XTESTUTILS_H)
#define XTESTUTILS_H



#define DBL_RAND() (((double) rand()) / (1.0 + (double) RAND_MAX))



//int xdlt_dump_mmfile(char const *fname, mmfile_t *mmf);
int xdlt_load_mmfile(char const *fname, mmfile_t *mf, int binmode);
//int xdlt_do_diff(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
//		 xdemitconf_t const *xecfg, mmfile_t *mfp);
//int xdlt_do_patch(mmfile_t *mfo, mmfile_t *mfp, int mode, mmfile_t *mfr);
//int xdlt_do_regress(mmfile_t *mf1, mmfile_t *mf2, xpparam_t const *xpp,
//		    xdemitconf_t const *xecfg);
//long xdlt_gen_line(char *buf, long msize);
//int xdlt_create_file(mmfile_t *mf, long size);
//int xdlt_change_file(mmfile_t *mfo, mmfile_t *mfr, double rmod, int chmax);
//int xdlt_auto_regress(xpparam_t const *xpp, xdemitconf_t const *xecfg, long size,
//		      double rmod, int chmax);
//int xdlt_do_bindiff(mmfile_t *mf1, mmfile_t *mf2, bdiffparam_t const *bdp, mmfile_t *mfp);
//int xdlt_do_rabdiff(mmfile_t *mf1, mmfile_t *mf2, mmfile_t *mfp);
//int xdlt_do_binpatch(mmfile_t *mf, mmfile_t *mfp, mmfile_t *mfr);
//int xdlt_do_binregress(mmfile_t *mf1, mmfile_t *mf2, bdiffparam_t const *bdp);
//int xdlt_do_rabinregress(mmfile_t *mf1, mmfile_t *mf2);
//int xdlt_auto_binregress(bdiffparam_t const *bdp, long size,
//			 double rmod, int chmax);
//int xdlt_auto_rabinregress(long size, double rmod, int chmax);
//int xdlt_auto_mbinregress(bdiffparam_t const *bdp, long size,
//			  double rmod, int chmax, int n);


#endif /* #if !defined(XTESTUTILS_H) */

