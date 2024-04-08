#include <stdio.h>

static const int rating[][4] = {
    {1344,7,841,655},
    {349,44,343,2649},
    {746,22,1245,147},
    {1293,341,231,38},
    {1730,1036,212,309},
    {14,1402,1521,1297},
    {583,618,1081,831},
    {2278,1047,857,1119},
    {87,1659,175,376},
    {149,524,1025,669},
    {1208,709,494,1877},
    {1791,1443,767,491},
    {571,3666,675,613},
    {1038,1625,418,240},
    {1365,2734,385,867},
    {2713,127,2637,428},
    {714,976,2819,520},
    {395,462,799,369},
    {2421,2679,81,2634},
    {542,1258,54,2221},
    {1628,125,767,934},
    {1329,58,1566,1925},
    {37,230,489,2915},
    {2842,1518,200,514},
    {1144,55,2330,6},
    {302,347,1906,2678},
    {94,1910,1554,2994},
    {2266,853,1224,468},
    {58,36,1316,1623},
    {42,690,742,3281},
    {389,757,469,511},
    {1402,136,1401,2003},
    {911,1732,1039,2192},
    {1468,1436,322,1027},
    {363,1849,404,1642},
    {2474,1690,1435,1554},
    {1600,2719,441,208},
    {229,587,355,307},
    {668,1438,203,47},
    {2002,576,650,2785},
    {2040,735,517,2140},
    {1586,337,924,528},
    {971,641,79,138},
    {2262,708,2570,630},
    {159,1081,2184,109},
    {1966,43,57,2215},
    {2219,2151,609,614},
    {897,212,2484,650},
    {131,569,1297,606},
    {1890,1920,757,417},
    {2953,702,3238,1299},
    {111,191,3139,361},
    {99,149,18,281},
    {232,170,942,2884},
    {1998,423,684,37},
    {2591,2,149,1553},
    {1538,359,774,280},
    {1062,243,38,1991},
    {729,1491,169,810},
    {304,3158,2124,1770},
    {203,139,885,58},
    {62,419,83,1739},
    {1046,448,112,2881},
    {133,897,658,2968},
    {1150,1829,1683,2455},
    {2215,1609,525,1292},
    {7,820,300,72},
    {159,2927,337,301},
    {1237,470,1637,2072},
    {1898,168,2929,775},
    {216,2823,108,51},
    {1508,803,63,62},
    {930,90,220,187},
    {1112,78,1668,725},
    {570,1180,2015,1112},
    {1943,378,2345,2763},
    {2427,258,600,441},
    {2317,708,1108,183},
    {169,153,1666,1157},
    {721,1581,1487,1527},
    {321,3244,1751,67},
    {1691,2264,86,46},
    {1561,494,152,1832},
    {383,2150,20,1507},
    {311,1190,679,744},
    {600,865,1200,445},
    {2019,80,886,80},
    {1031,1503,390,27},
    {122,3010,947,2248},
    {1075,2302,620,1920},
    {390,653,837,415},
    {46,1948,116,1161},
    {1385,603,903,272},
    {1320,169,393,165},
    {1338,505,25,233},
    {220,1431,216,1804},
    {246,2975,1265,108},
    {652,1410,1054,614},
    {232,2082,2317,1503},
    {1697,225,24,313},
    {140,885,2597,1326},
    {789,1925,228,518},
    {456,902,81,1581},
    {2241,2249,629,1443},
    {30,289,909,542},
    {455,33,547,396},
    {629,471,1246,647},
    {1,1178,33,320},
    {358,1131,2958,773},
    {21,449,1148,1841},
    {1282,759,662,1436},
    {1668,466,997,3275},
    {150,3273,2261,1610},
    {2334,1559,1846,1977},
    {1388,321,1549,312},
    {1936,84,2290,1447},
    {877,1088,1209,286},
    {905,1204,540,2556},
    {505,1002,1827,1102},
    {250,881,1345,1985},
    {843,40,1618,1871},
    {485,1375,598,67},
    {40,1653,414,389},
    {598,320,202,2110},
    {1161,1336,1170,632},
    {487,227,234,2130},
    {157,654,1779,1091},
    {372,1982,238,131},
    {2305,1274,1266,2316},
    {280,13,3000,2368},
    {2114,3767,900,992},
    {2981,1005,1574,735},
    {2103,690,1271,2328},
    {175,1216,152,215},
    {39,1554,1293,3521},
    {1504,1077,517,619},
    {2679,770,2796,967},
    {1,1854,853,8},
    {1538,1407,873,1696},
    {85,2806,1525,426},
    {312,1626,471,81},
    {1428,656,485,1373},
    {204,430,2401,744},
    {2908,333,1120,72},
    {142,382,1228,12},
    {1132,2874,290,1321},
    {1048,1243,722,1280},
    {1502,278,905,210},
    {2785,676,423,2215},
    {1081,211,334,803},
    {910,20,621,378},
    {48,1739,324,1457},
    {567,905,1612,491},
    {2625,1047,631,521},
    {1877,1100,2296,994},
    {610,369,781,126},
    {2296,3098,759,627},
    {2390,1107,699,408},
    {187,2415,1346,493},
    {972,73,724,928},
    {1043,118,64,653},
    {620,537,2541,150},
    {918,160,53,230},
    {313,655,2019,128},
    {1373,615,1832,1137},
    {191,1495,476,267},
    {646,458,2324,26},
    {58,157,2613,781},
    {272,294,2026,1422},
    {7,313,213,789},
    {352,75,2648,437},
    {193,1677,160,1863},
    {1188,832,757,334},
    {563,312,1435,20},
    {940,558,954,557},
    {567,899,829,2068},
    {657,168,204,1320},
    {2212,2918,461,428},
    {149,898,6,981},
    {363,1021,852,2766},
    {18,131,290,534},
    {1502,37,1440,579},
    {87,86,495,1699},
    {668,1952,742,902},
    {300,1201,835,385},
    {2798,206,1230,548},
    {577,718,31,2703},
    {33,1952,360,827},
    {1320,2522,15,810},
    {288,505,308,2551},
    {1850,2090,3001,313},
    {814,185,792,320},
    {675,1278,3606,2682},
    {85,120,2269,1983},
    {86,159,1858,2377},
    {2523,797,2783,1324},
    {270,600,62,39},
    {650,1989,1351,470},
    {1533,297,3,2810},
    {3115,1584,357,1250},
};
static const int N = sizeof rating / sizeof *rating;

static int eval(const int x, const int m, const int a, const int s)
{
    goto in;
fxr:
    if (x > 387) return 0;
    if (a < 752) return 0;
    if (s > 1381) goto fb;
    goto sn;
dds:
    if (m < 1139) goto jr;
    if (s < 1864) goto jct;
    return 0;
dt:
    if (m < 2896) goto vr;
    goto xt;
sz:
    if (s < 1606) goto cnp;
    return 0;
zzb:
    if (m < 3467) goto rnc;
    if (s > 3815) return 0;
    goto qqs;
kh:
    if (m < 3815) return 1;
    if (s > 592) return 0;
    return 1;
jj:
    if (x < 2953) goto sfj;
    if (m > 549) return 0;
    return 1;
qzl:
    if (x > 2819) goto gvl;
    if (s < 3517) goto xz;
    if (s < 3728) goto htx;
    goto fsq;
xz:
    if (a < 1471) goto gxc;
    goto mp;
dlv:
    if (s > 1732) return 1;
    if (x > 704) goto tzs;
    goto fq;
jh:
    return 0;
tzn:
    if (x < 275) return 0;
    if (m > 3397) return 0;
    goto kcv;
vdl:
    if (s < 1312) goto ppg;
    if (s < 1688) goto bd;
    if (m > 1062) goto jmk;
    goto zbh;
zm:
    if (m < 2770) goto vb;
    goto thf;
dxd:
    if (a > 3268) return 0;
    if (m < 3138) return 0;
    if (s > 515) return 0;
    return 1;
mrv:
    if (a < 3117) return 1;
    return 0;
ngd:
    if (m < 796) return 1;
    if (x > 809) return 1;
    if (x > 685) return 0;
    return 1;
fzd:
    if (s > 1412) goto qcj;
    return 1;
xtl:
    if (m < 2980) goto kjb;
    if (m < 3130) goto dxn;
    return 0;
fsg:
    if (s > 1661) goto rm;
    goto zmt;
zt:
    if (m < 3631) return 1;
    if (a > 993) return 1;
    return 0;
bn:
    if (s > 2462) return 1;
    return 0;
znn:
    if (s < 2343) goto mb;
    goto kzn;
hgd:
    if (s < 2100) goto cx;
    return 1;
bgk:
    if (x < 3493) return 0;
    goto dl;
kq:
    if (x < 1598) goto crq;
    if (m > 3282) goto brb;
    return 1;
mvb:
    if (x > 1700) return 1;
    return 0;
dfg:
    if (a > 1208) goto brc;
    if (m < 952) goto hmk;
    goto tmk;
pcb:
    if (m > 710) goto zrc;
    if (x > 2685) return 1;
    goto tdg;
bqq:
    if (m > 1206) goto ckf;
    if (s < 3406) goto nkt;
    if (m < 739) goto fqg;
    goto fch;
xrg:
    if (a > 1435) return 0;
    if (x > 3728) return 1;
    if (x < 3711) goto knr;
    goto zhj;
gd:
    if (s < 639) return 0;
    if (x > 800) return 0;
    return 1;
ds:
    if (s < 3740) goto zqv;
    if (x < 3251) return 0;
    if (a > 1748) goto lb;
    goto fl;
ff:
    if (s > 3133) return 1;
    if (m > 2918) return 0;
    return 1;
klt:
    if (s < 780) return 1;
    if (x < 2427) return 0;
    return 1;
hs:
    if (x > 2064) return 0;
    return 1;
qcj:
    if (s < 1497) return 1;
    if (s < 1512) return 1;
    return 0;
kc:
    if (s < 1570) return 1;
    return 0;
lzr:
    if (a < 2023) return 0;
    return 1;
mms:
    if (s > 2760) return 1;
    if (m < 916) goto zb;
    goto mq;
kks:
    if (s > 1321) goto df;
    if (x > 1286) goto mfj;
    goto hgx;
dh:
    if (x < 2779) return 1;
    return 0;
bc:
    return 0;
hn:
    if (s > 2717) return 0;
    if (m > 3726) goto fkv;
    if (m > 3413) goto sq;
    return 0;
cx:
    if (s > 778) return 1;
    if (m > 1497) return 0;
    return 1;
gvn:
    if (a < 3079) return 0;
    if (m < 3064) return 0;
    return 1;
lsm:
    if (a < 3319) goto lz;
    if (m < 929) goto xc;
    return 0;
jv:
    if (m < 276) return 1;
    return 0;
nmr:
    if (x < 3119) goto cl;
    goto xqj;
hnj:
    if (m < 554) goto zx;
    if (s > 2350) return 1;
    if (x < 3578) goto cxl;
    goto rbh;
tr:
    return 1;
zr:
    return 1;
fn:
    if (x < 3543) return 1;
    return 0;
jpz:
    return 1;
hq:
    if (s < 1702) goto mqn;
    if (x < 3035) goto cqm;
    if (m > 866) goto sj;
    goto lzr;
lz:
    if (a > 2980) return 0;
    if (s < 3374) return 1;
    if (m < 1157) return 1;
    return 0;
hrx:
    if (a > 869) return 0;
    if (m < 2843) return 1;
    if (s < 2361) return 0;
    return 1;
sjd:
    if (s > 3443) return 1;
    if (a < 3057) goto nvj;
    return 1;
xb:
    if (x > 920) goto lpj;
    return 1;
pxh:
    return 1;
qd:
    if (x > 1997) return 0;
    if (s > 2030) return 0;
    if (x > 1917) return 1;
    return 0;
xl:
    if (a < 192) goto rv;
    goto vdr;
jbx:
    if (a > 3649) return 0;
    goto mvb;
vh:
    if (a > 1478) return 0;
    return 1;
nd:
    if (x > 2524) return 1;
    if (m < 1268) return 0;
    return 1;
zrc:
    return 1;
hjk:
    if (s > 1250) goto dnn;
    if (a > 1163) goto ndf;
    if (m > 1379) goto zcv;
    goto rjb;
bp:
    if (s > 1723) goto mxk;
    goto hjp;
nvj:
    if (s < 2798) return 0;
    if (m < 1158) return 0;
    if (s > 3174) return 1;
    return 0;
rxx:
    if (x < 2792) return 1;
    return 0;
kdj:
    return 1;
tzs:
    return 0;
fr:
    return 0;
bvq:
    if (x < 3598) goto qxz;
    if (x > 3763) goto npf;
    if (x < 3701) goto cc;
    goto xrg;
clj:
    if (s < 3489) goto qt;
    if (a > 1043) return 0;
    if (s < 3828) return 0;
    return 1;
brb:
    return 0;
fnd:
    if (a > 1376) return 0;
    if (m > 2369) return 1;
    return 0;
xhx:
    if (a > 1184) return 0;
    if (s < 2676) return 0;
    if (m < 3079) return 0;
    return 1;
zrk:
    if (x < 451) goto nb;
    return 1;
dhg:
    if (s < 3172) return 1;
    if (a > 1399) return 0;
    return 1;
tg:
    if (x < 1271) goto nk;
    goto zdg;
mm:
    if (s > 2916) goto gg;
    goto rxj;
hpx:
    if (a > 862) return 1;
    if (m < 787) goto vsv;
    if (a < 807) goto cfh;
    goto xpb;
fts:
    if (x < 2853) goto dds;
    if (m > 1519) goto sx;
    if (s < 1369) goto dhz;
    goto mms;
sdk:
    if (s < 1095) return 1;
    if (s < 1756) return 0;
    return 1;
tmk:
    if (x < 3193) return 0;
    return 1;
drj:
    if (m > 1575) goto hpd;
    if (x > 605) goto hgd;
    if (s < 1335) goto kgm;
    goto fg;
jlt:
    return 0;
vsq:
    if (x < 1755) return 1;
    if (s < 2371) return 0;
    return 1;
pc:
    if (a > 2493) goto rg;
    if (a > 2002) goto gq;
    goto bp;
svq:
    if (m > 2556) return 1;
    if (x > 2672) return 1;
    return 0;
sqh:
    if (a < 1445) goto js;
    if (s > 1591) return 0;
    if (s > 1477) return 0;
    return 1;
grb:
    if (s > 2580) goto lsm;
    if (a < 3313) goto rt;
    goto hk;
rqg:
    return 1;
zpd:
    if (s > 794) return 1;
    if (x < 2667) return 1;
    return 0;
rqt:
    if (a > 1658) goto mx;
    if (a < 777) goto dxq;
    if (s < 2616) goto hjk;
    goto pkm;
gtb:
    if (s > 3730) return 0;
    return 1;
zlh:
    if (x < 1712) return 0;
    if (a > 427) return 0;
    if (x < 1771) goto sdk;
    goto cm;
td:
    if (m > 3499) return 1;
    if (m > 3414) goto gt;
    if (a < 494) return 1;
    goto kgq;
dgt:
    if (a > 825) goto ct;
    goto pxh;
km:
    if (s > 654) goto nqs;
    goto nmn;
ppg:
    return 1;
slk:
    if (x > 2763) return 1;
    return 0;
trv:
    if (x > 539) goto sjd;
    if (x < 264) goto cfd;
    if (a > 2787) goto bqq;
    goto lpr;
snv:
    if (s > 305) return 1;
    if (x < 556) return 1;
    if (s < 183) return 0;
    return 1;
gg:
    if (s < 3016) return 1;
    if (s < 3084) return 1;
    return 0;
dns:
    if (m > 3482) goto fzd;
    goto xvg;
sq:
    return 1;
pvk:
    if (s > 1304) return 1;
    if (m < 2685) return 1;
    return 0;
rxq:
    if (m > 2599) goto qg;
    if (m < 2492) goto bq;
    goto zq;
pn:
    if (a < 589) return 0;
    if (m > 1655) return 1;
    if (a > 992) return 0;
    return 1;
lv:
    if (m < 3175) goto vf;
    if (a < 3191) goto vt;
    if (x < 751) goto cn;
    goto jbx;
ct:
    if (s < 558) goto snv;
    if (a < 1249) goto dzx;
    goto jvt;
dzx:
    if (m < 2863) return 0;
    return 1;
js:
    return 1;
fpr:
    if (a < 986) return 0;
    if (x < 2658) return 0;
    return 1;
gz:
    if (a < 1509) return 1;
    if (x < 3783) return 1;
    return 0;
zjm:
    return 1;
xsm:
    if (a < 1660) return 0;
    if (a < 1844) return 0;
    return 1;
mls:
    if (m > 424) goto ln;
    goto zrk;
grj:
    return 1;
kn:
    if (s > 142) return 0;
    if (a < 1016) return 0;
    return 1;
mxk:
    if (a < 1780) return 1;
    if (a > 1859) return 0;
    if (m > 3461) return 1;
    return 0;
vn:
    if (s < 1454) goto sl;
    return 0;
njz:
    if (m < 1859) return 0;
    return 1;
dzk:
    if (x > 3558) goto kcd;
    goto nf;
zsk:
    if (a > 476) return 1;
    if (x > 530) goto ngd;
    if (x > 267) goto cfl;
    return 1;
zd:
    if (m > 616) return 0;
    if (s > 838) goto ddb;
    return 1;
qxk:
    if (m > 2824) goto qkg;
    goto qx;
nrr:
    if (m > 1084) return 1;
    if (s > 971) return 0;
    return 1;
hm:
    if (m > 3025) goto vfv;
    goto nz;
lpj:
    if (m > 3793) return 0;
    if (s > 1805) return 1;
    return 0;
mf:
    if (x < 2463) return 0;
    if (s < 3847) goto zjt;
    goto rqg;
gvj:
    if (s > 223) goto zkv;
    if (x < 3876) goto kn;
    if (a > 1010) return 0;
    goto dr;
fbs:
    if (x < 3091) goto qdb;
    goto mxf;
psz:
    if (m > 3320) goto fpr;
    if (m > 3250) return 0;
    if (x < 2725) goto tq;
    return 0;
jhj:
    return 1;
gt:
    if (m < 3463) return 0;
    if (x < 722) return 1;
    return 0;
hx:
    return 1;
kg:
    if (x > 3062) goto jsh;
    return 0;
bxh:
    if (m > 1510) return 1;
    if (a > 677) return 1;
    return 0;
vfv:
    if (s > 3630) return 1;
    if (a < 1123) return 1;
    return 0;
tv:
    if (s > 2870) goto pmp;
    if (m > 1164) goto qj;
    goto bc;
bnz:
    if (s > 2128) goto ktr;
    goto vdl;
cjd:
    return 0;
rf:
    if (a < 1243) return 0;
    if (m > 2603) return 0;
    return 1;
mfs:
    return 1;
xh:
    if (m > 3128) return 0;
    if (s < 3715) return 0;
    return 1;
jp:
    if (a > 2848) goto fhg;
    if (s > 2879) return 1;
    return 0;
rh:
    if (s < 180) goto xst;
    if (a > 1234) goto xsm;
    goto qnj;
ndh:
    return 1;
qxz:
    if (m > 1434) goto xr;
    if (s < 417) goto jds;
    if (x > 3320) return 1;
    goto ffq;
sdj:
    if (a > 703) return 0;
    return 1;
csm:
    if (x > 2471) return 0;
    if (s > 3581) return 1;
    return 0;
ps:
    return 1;
mnl:
    if (s < 3342) goto xjb;
    if (x > 2880) goto zn;
    goto hg;
sg:
    if (s > 3292) goto vl;
    goto vsz;
nc:
    if (s > 863) return 0;
    return 1;
fv:
    if (a > 724) return 0;
    if (a < 329) return 0;
    return 1;
qr:
    if (a > 985) return 0;
    if (x > 2015) return 0;
    if (x > 1932) return 0;
    return 1;
rdl:
    if (s < 1344) return 1;
    if (s > 2529) goto mj;
    if (s > 2040) return 1;
    return 0;
xc:
    if (m < 479) return 0;
    if (m > 684) return 0;
    if (a < 3738) return 1;
    return 0;
zbh:
    if (m < 549) return 0;
    if (a < 2124) return 0;
    if (a < 3008) goto xx;
    goto vgc;
md:
    return 1;
vs:
    if (a < 3669) return 0;
    if (s < 2516) return 0;
    if (m > 3794) return 1;
    return 0;
zj:
    return 0;
mqn:
    if (m > 1072) goto mfs;
    if (a > 2014) return 1;
    if (m > 664) goto jrv;
    goto nsk;
fkv:
    if (m > 3849) return 1;
    if (s < 2506) return 1;
    return 0;
lxq:
    if (a > 1465) return 0;
    if (s < 3588) return 1;
    if (x > 2736) return 0;
    return 1;
zrn:
    if (m < 2517) goto qjr;
    if (s < 2143) goto gc;
    goto zs;
kx:
    if (x < 2618) goto gcr;
    if (a > 458) goto rrm;
    goto slm;
crq:
    return 0;
ntx:
    return 1;
llr:
    if (x > 1921) return 1;
    return 0;
hcr:
    if (s > 3586) return 0;
    if (x < 2442) return 0;
    return 1;
hzs:
    return 0;
jkv:
    if (m < 3505) return 0;
    if (s < 3505) return 1;
    return 0;
xk:
    if (m > 2872) return 1;
    return 0;
fq:
    return 1;
mhr:
    if (s < 533) return 0;
    if (m < 3751) return 0;
    if (m < 3850) return 0;
    return 1;
rbh:
    if (a < 507) return 0;
    return 1;
tnl:
    if (a < 962) return 0;
    if (m > 1767) return 1;
    return 0;
qtk:
    return 1;
gvl:
    if (s < 3240) goto dbz;
    if (x > 3507) goto nx;
    if (a < 1452) goto kg;
    goto vj;
xj:
    return 1;
tlp:
    if (x > 2776) return 0;
    return 1;
tq:
    return 0;
zz:
    if (x > 362) goto tk;
    if (a < 688) return 0;
    return 1;
rt:
    if (m < 1209) return 1;
    if (a > 3063) goto hzs;
    if (a > 2949) return 1;
    return 0;
kcv:
    return 0;
ddb:
    if (x > 3833) return 0;
    if (a > 970) return 0;
    if (m > 251) return 0;
    return 1;
hk:
    if (m > 1324) goto njz;
    return 1;
sm:
    if (s < 2928) return 1;
    if (x < 2433) return 1;
    return 0;
vr:
    if (s < 2227) goto qsq;
    if (x > 3540) goto qc;
    return 1;
tn:
    if (s < 3175) return 0;
    if (s < 3690) return 1;
    return 0;
ln:
    if (s < 3229) return 1;
    if (s > 3702) goto fv;
    goto cj;
hql:
    if (m < 2948) goto lh;
    if (m < 3270) goto vlh;
    goto nmr;
txr:
    if (a < 949) goto chl;
    goto xcv;
zdg:
    if (s < 880) goto qq;
    goto mrv;
mb:
    if (m > 2828) goto md;
    if (x > 3659) goto mhl;
    return 0;
slm:
    if (s < 3600) return 0;
    if (a > 231) goto txt;
    return 0;
in:
    if (x > 2187) goto rqt;
    goto zrn;
cdj:
    return 1;
zn:
    if (x < 3349) return 0;
    if (s > 3779) return 1;
    return 0;
nh:
    if (m > 3074) return 1;
    if (s > 3081) goto nfz;
    if (s > 2486) goto jlt;
    goto hrx;
nxh:
    if (s < 467) goto rh;
    goto sp;
ml:
    return 0;
pj:
    if (a > 1449) return 1;
    goto fnd;
xkx:
    if (m > 478) goto gx;
    if (s < 1114) goto kb;
    return 0;
tf:
    if (x < 2576) goto lmr;
    if (m > 3034) return 0;
    goto qmn;
dr:
    return 1;
gbz:
    if (x > 3659) return 0;
    return 1;
cj:
    return 1;
tfj:
    if (s < 3594) return 0;
    if (s > 3615) return 0;
    if (x > 2437) return 0;
    return 1;
ckf:
    if (a > 3590) return 0;
    if (x > 438) return 1;
    if (s < 3110) return 0;
    return 1;
rkv:
    if (s < 958) return 1;
    if (x > 2427) return 0;
    return 1;
gs:
    return 1;
pv:
    if (s < 1719) return 0;
    if (a > 185) return 0;
    return 1;
kd:
    if (m > 3623) return 0;
    return 1;
vt:
    if (x < 774) return 1;
    if (m > 3551) goto mrs;
    return 0;
dzl:
    if (m > 1548) return 0;
    if (m < 832) return 1;
    return 0;
dbz:
    if (m > 2501) goto hp;
    if (m > 1237) goto fn;
    if (x < 3271) return 0;
    return 1;
vl:
    if (s > 3563) return 1;
    return 0;
sp:
    if (x > 2041) goto jjj;
    if (m > 3338) goto pd;
    if (a > 672) return 0;
    goto ns;
mcc:
    if (a > 843) goto kfc;
    if (m < 3321) goto xtl;
    if (m < 3672) goto td;
    goto xb;
sgv:
    if (x < 2402) goto qgn;
    if (a < 1075) return 1;
    goto csm;
bst:
    if (a > 1595) goto jjx;
    goto nh;
dnn:
    if (a < 1179) goto fbs;
    goto ck;
lm:
    if (m > 1644) return 0;
    if (m > 950) return 0;
    if (m > 453) return 1;
    return 0;
jds:
    if (x > 3345) return 0;
    return 1;
xr:
    if (x > 3440) return 1;
    if (x > 3338) return 1;
    if (m > 2689) return 0;
    return 1;
sl:
    if (s < 1413) return 0;
    if (m > 3475) return 1;
    if (s < 1439) return 1;
    return 0;
htb:
    if (s > 3626) return 1;
    if (s < 3564) goto dpm;
    if (a < 1533) goto smk;
    goto tfj;
hh:
    if (m > 3184) goto pz;
    if (x < 1598) goto hth;
    if (s < 1273) return 0;
    goto kdj;
df:
    if (s < 1339) return 0;
    if (a > 712) return 0;
    return 1;
fl:
    if (m < 373) return 0;
    if (m < 825) return 0;
    return 1;
cqs:
    if (m > 3246) goto hn;
    if (a > 438) goto mmj;
    if (s > 2690) goto mm;
    goto bdg;
cm:
    if (m > 3143) return 1;
    if (a < 271) return 0;
    return 1;
lnp:
    if (s < 2823) return 0;
    if (a > 1198) return 0;
    return 1;
dpm:
    if (x < 2406) return 1;
    return 0;
rnc:
    return 1;
kf:
    if (x < 2523) return 1;
    goto gsc;
st:
    if (a < 984) return 1;
    if (x > 3040) return 0;
    return 1;
grh:
    if (a < 968) goto vsq;
    if (x < 1710) return 0;
    if (x > 1980) goto pft;
    return 1;
mxf:
    if (m > 2589) return 0;
    return 1;
tlr:
    if (x > 212) return 0;
    if (m < 1300) goto lmz;
    if (x > 134) return 0;
    return 1;
pmp:
    if (s > 2984) return 0;
    if (x < 2750) return 0;
    return 1;
kgm:
    if (s > 684) goto bxh;
    goto sdj;
nz:
    if (s > 3680) return 1;
    if (x < 2711) return 0;
    return 1;
hj:
    if (m > 468) return 0;
    if (a > 1334) return 1;
    goto jpz;
nsk:
    return 0;
gc:
    if (a > 1942) goto tg;
    if (x > 1452) goto cvq;
    if (s < 1213) goto kps;
    goto rlm;
rjb:
    if (x > 3046) goto rcm;
    goto bzs;
nn:
    if (a > 893) return 1;
    return 0;
dpt:
    if (s > 3560) return 0;
    goto qhc;
vkn:
    if (s > 3622) return 1;
    return 0;
kl:
    if (s > 3176) return 0;
    return 1;
zmt:
    if (m < 3605) goto fck;
    return 1;
bd:
    if (s > 1443) return 0;
    goto xj;
jjx:
    if (s < 3339) goto lfz;
    if (s > 3750) goto srf;
    return 1;
rkt:
    if (s > 1835) return 1;
    if (x > 2035) return 0;
    if (m < 3485) return 1;
    return 0;
nhc:
    return 0;
mrs:
    return 1;
bq:
    if (x > 2850) return 1;
    if (x > 2439) return 1;
    return 0;
rxn:
    if (s > 2537) return 0;
    if (x > 2737) return 0;
    return 1;
nzg:
    if (m < 636) return 0;
    return 1;
rpj:
    if (s < 1966) return 1;
    if (s > 2074) return 1;
    if (x > 2072) return 1;
    return 0;
xf:
    if (s < 815) goto msz;
    if (m < 1861) goto sz;
    goto hcq;
qx:
    if (m < 2037) goto bf;
    if (m < 2524) return 0;
    return 1;
rxj:
    if (s < 2816) return 0;
    return 1;
zf:
    if (x < 2538) goto jc;
    if (m < 3187) goto hm;
    goto psz;
vv:
    if (a < 1290) goto nrr;
    if (m > 1076) goto slk;
    if (a > 1321) goto hx;
    return 1;
px:
    if (x > 115) return 0;
    return 1;
jr:
    if (m < 598) return 0;
    if (x > 2591) return 0;
    return 1;
kjb:
    if (a < 384) return 1;
    if (a > 621) return 1;
    return 0;
hz:
    if (s < 2399) goto ffj;
    goto trv;
vf:
    if (x < 945) return 1;
    goto bn;
lf:
    if (m > 1109) goto hsd;
    if (s < 2503) goto sh;
    if (s < 3454) goto jj;
    goto ds;
hsd:
    if (s < 1932) return 1;
    if (x < 3025) goto ch;
    return 1;
jtr:
    if (s < 142) return 0;
    if (a > 1018) goto cr;
    goto qss;
vg:
    if (m < 3527) return 1;
    return 0;
jrv:
    if (a > 1928) return 0;
    if (x < 3118) return 1;
    if (a > 1902) return 0;
    return 1;
qdb:
    if (x > 2536) goto tnl;
    goto jrn;
rz:
    if (m > 3013) goto pc;
    goto rxq;
jnd:
    if (a < 699) return 0;
    if (s > 1253) return 1;
    if (a > 1511) return 0;
    return 1;
xcv:
    if (m < 3557) return 1;
    if (a > 1498) goto fr;
    if (m > 3708) goto kh;
    goto gd;
nr:
    if (s > 498) goto dh;
    if (x > 2708) return 0;
    return 1;
vlh:
    if (s > 2857) return 0;
    if (a < 981) return 0;
    if (s > 2724) goto zk;
    goto xhx;
qkg:
    if (a < 943) goto nbb;
    goto zjm;
pzg:
    if (m > 3036) goto gp;
    goto pk;
qvg:
    if (m < 3693) goto sk;
    if (s < 3471) goto pvg;
    goto jcb;
jc:
    if (a > 1089) return 1;
    goto xh;
vch:
    if (a < 1432) goto zpd;
    if (m < 2773) goto km;
    if (s > 505) goto fhn;
    goto kf;
xzk:
    if (m > 2079) return 1;
    if (x < 470) return 0;
    return 1;
rlm:
    if (s > 1542) goto mcc;
    if (x < 662) goto pzg;
    if (x > 1156) goto lgp;
    goto dns;
dfc:
    if (m > 2382) return 0;
    return 1;
pvc:
    if (s < 2759) return 1;
    if (s > 3572) return 1;
    return 0;
vb:
    if (s > 1675) goto qr;
    if (m < 2628) return 1;
    if (x < 1961) goto pvk;
    goto hs;
vgc:
    if (s > 1902) return 0;
    return 1;
knn:
    if (x > 2559) return 1;
    return 0;
pls:
    if (a > 1057) return 1;
    return 0;
sfj:
    if (x > 2695) return 1;
    return 0;
cvq:
    if (x < 1846) goto tj;
    if (s < 1069) goto nxh;
    if (m > 3148) goto fsg;
    goto zm;
npf:
    if (a > 1493) return 1;
    if (a < 1330) return 1;
    if (a < 1421) return 1;
    return 0;
qgn:
    if (x < 2318) return 0;
    if (m < 2680) return 1;
    return 0;
ck:
    if (s > 2096) goto pdc;
    if (x > 3055) goto bx;
    if (m > 1372) goto tf;
    goto sqh;
fck:
    if (s < 1426) return 0;
    return 1;
bk:
    if (s > 2778) return 0;
    if (m > 844) return 0;
    return 1;
drx:
    if (x < 2517) goto sgv;
    goto dpt;
pq:
    return 0;
zcv:
    if (s < 576) goto vrs;
    goto qxk;
mj:
    return 1;
dfk:
    return 0;
knr:
    return 0;
szf:
    return 0;
hjp:
    if (a < 1852) return 1;
    return 0;
qf:
    if (x < 753) goto bst;
    goto sg;
nmn:
    if (a < 1510) return 1;
    if (m < 2097) return 1;
    return 0;
zq:
    if (x > 2996) return 1;
    if (a < 3146) goto lts;
    goto svq;
smk:
    return 1;
ph:
    if (s > 677) return 1;
    if (a > 963) goto mv;
    if (s < 316) goto nn;
    return 1;
vrs:
    if (s < 292) goto jtr;
    goto vdk;
vzq:
    return 1;
dxz:
    return 0;
hgx:
    if (m < 3378) return 0;
    if (x < 1215) return 0;
    return 1;
kzn:
    if (x < 3718) goto fxt;
    if (x > 3812) return 1;
    if (a < 649) goto nzc;
    return 1;
zgq:
    if (a > 1606) return 0;
    return 1;
sh:
    if (a > 1766) goto ml;
    if (m > 411) return 1;
    if (x > 3272) goto ndh;
    goto ntx;
jmk:
    if (m > 1801) goto dmh;
    if (a < 2515) return 0;
    goto qtk;
qc:
    if (x > 3775) return 0;
    if (a < 254) return 0;
    return 1;
vz:
    if (a < 1402) return 1;
    return 0;
kps:
    if (m < 3310) goto dgt;
    goto txr;
chl:
    if (x < 880) goto cdj;
    if (m > 3584) goto mhr;
    goto grj;
hnt:
    return 1;
vrq:
    if (m < 391) return 1;
    if (s > 728) return 1;
    return 0;
xvg:
    if (a > 799) return 1;
    if (x < 882) goto hnt;
    goto xqq;
kfd:
    return 0;
lgc:
    if (m < 859) goto llr;
    return 0;
hlc:
    if (a > 1087) return 1;
    return 0;
kfc:
    if (a < 1396) return 1;
    if (s < 1784) return 1;
    goto tsc;
cr:
    return 0;
bdg:
    if (m > 2790) goto rxn;
    return 1;
nfz:
    if (x < 462) return 0;
    return 1;
tc:
    return 1;
sr:
    if (a > 1117) goto dfg;
    if (a < 915) goto lcl;
    goto mnl;
cn:
    if (m > 3623) goto vs;
    if (x > 327) goto stz;
    return 1;
qq:
    if (a < 2978) goto hxd;
    if (a > 3368) return 0;
    if (a < 3205) goto gvn;
    goto dxd;
vj:
    if (m < 1523) return 1;
    goto mcq;
tsc:
    return 1;
vsz:
    if (x > 1670) return 0;
    if (m < 3429) goto mdc;
    goto xn;
clt:
    if (x > 3309) return 0;
    if (x < 3040) return 0;
    return 1;
fxt:
    return 0;
htx:
    if (x > 2607) goto gvq;
    if (m < 1444) goto htb;
    goto pj;
fjs:
    if (m < 2300) return 1;
    if (m < 2373) return 1;
    goto px;
nbb:
    if (a < 878) return 1;
    return 0;
rrm:
    if (a < 652) return 0;
    goto pvj;
dxn:
    if (a < 400) return 0;
    return 1;
fhn:
    if (a > 1534) goto zgq;
    if (m < 3262) goto dgx;
    if (m > 3663) return 0;
    goto nc;
gj:
    if (s < 3492) return 1;
    return 0;
lcl:
    if (a > 848) goto qv;
    if (x < 2916) goto zcp;
    goto kz;
rrz:
    if (m < 1945) return 0;
    if (s > 358) return 0;
    return 1;
sj:
    if (s < 3166) goto njn;
    goto hf;
thf:
    if (s < 1521) return 1;
    if (m > 2938) goto dfk;
    if (a < 1001) goto xk;
    goto bs;
lts:
    if (a < 2459) return 0;
    if (m < 2530) return 1;
    if (x < 2496) return 1;
    return 0;
fsq:
    if (m < 1600) goto mf;
    goto dmj;
dxg:
    if (s > 2375) goto mls;
    goto cp;
qz:
    if (m < 2151) goto lqj;
    if (x > 537) goto dlv;
    if (x > 300) goto fxr;
    goto fjs;
sf:
    if (s < 541) goto hj;
    if (a > 1380) goto bt;
    if (m < 538) goto mfb;
    goto vv;
tdg:
    if (a < 1031) return 1;
    return 0;
pft:
    if (x < 2060) return 0;
    if (a < 1308) return 0;
    if (s < 2438) return 0;
    return 1;
rmd:
    if (x > 2881) goto lk;
    goto qvg;
cfh:
    return 0;
srf:
    if (a > 2141) return 1;
    if (x < 268) return 1;
    return 0;
qj:
    if (m < 1485) return 0;
    return 1;
qvx:
    if (s < 3612) goto gj;
    goto bgk;
msz:
    if (m > 1978) goto tlp;
    goto rxx;
pkm:
    if (a > 1336) goto qzl;
    if (m < 2346) goto sr;
    if (m > 3445) goto rmd;
    goto pkh;
jrn:
    if (m < 1804) return 0;
    if (m < 2588) return 0;
    return 1;
nqs:
    return 1;
rtx:
    if (m < 1176) return 0;
    if (m < 1507) goto bcn;
    if (a > 292) goto vjm;
    goto zr;
lgp:
    if (s > 1365) goto vn;
    if (s > 1283) goto kks;
    goto jnd;
cl:
    return 1;
jct:
    if (m < 1542) return 0;
    if (s > 897) return 1;
    if (m > 1926) return 0;
    return 1;
gp:
    if (m < 3394) return 0;
    if (s > 1431) return 1;
    goto szf;
nf:
    if (a > 545) return 1;
    if (a > 415) return 1;
    return 0;
bx:
    if (m < 1737) return 1;
    goto vh;
mdc:
    if (s < 2717) return 0;
    return 1;
gth:
    if (m < 900) return 1;
    return 0;
jjj:
    if (s < 668) return 1;
    if (m < 3223) return 1;
    if (x > 2117) return 0;
    return 1;
qqs:
    if (x > 1370) return 0;
    if (x < 634) return 1;
    if (s < 3751) return 0;
    return 1;
cfd:
    if (m > 1163) goto tt;
    if (m < 540) goto jv;
    if (x < 125) return 1;
    return 0;
hcl:
    return 1;
fz:
    if (s > 2594) goto tv;
    goto rtx;
zkv:
    if (m > 688) return 0;
    if (m > 414) return 0;
    return 1;
tk:
    return 1;
fb:
    return 1;
cxl:
    if (a < 593) return 1;
    if (a > 657) return 0;
    if (m < 880) return 1;
    return 0;
xn:
    if (m < 3765) return 0;
    return 1;
vdk:
    if (m > 2574) return 0;
    if (s > 477) goto st;
    goto rrz;
zs:
    if (a > 2464) goto nl;
    goto qf;
xqq:
    if (s > 1384) return 1;
    if (a < 354) return 1;
    if (m > 3046) return 1;
    return 0;
nzc:
    if (a < 598) return 1;
    return 0;
pct:
    if (s > 2183) return 0;
    if (a < 1518) return 0;
    return 1;
nl:
    if (s > 3041) goto fvr;
    goto lv;
sk:
    if (s > 3236) goto kfd;
    return 0;
lmr:
    if (m < 2601) return 0;
    if (a > 1422) return 1;
    return 0;
zsj:
    if (s < 2295) goto xf;
    if (s > 3148) goto kx;
    if (m > 1925) goto cqs;
    goto fz;
rv:
    if (x < 3564) return 1;
    if (s > 2553) return 1;
    return 0;
cqb:
    if (m > 1544) return 1;
    if (m < 1535) return 1;
    if (s > 2351) return 1;
    return 0;
qnj:
    return 0;
gsc:
    if (a < 1578) return 1;
    return 0;
dhz:
    if (m > 953) return 0;
    goto vrq;
ffq:
    return 0;
zqv:
    return 0;
pk:
    if (s > 1393) goto hcl;
    if (m < 2713) return 0;
    if (x < 325) goto hlc;
    return 0;
mhl:
    if (s < 1312) return 1;
    if (m > 2356) return 0;
    return 1;
qhc:
    return 1;
vsv:
    return 0;
mcq:
    if (a > 1556) return 1;
    if (s < 3584) return 1;
    if (m < 3077) return 0;
    return 1;
ss:
    if (s > 785) return 1;
    if (s > 309) return 0;
    if (a < 991) return 1;
    goto tr;
xpb:
    if (m < 1158) return 1;
    if (a > 840) return 0;
    return 1;
gcr:
    if (s < 3558) goto gs;
    return 0;
rg:
    if (a > 3475) return 1;
    return 0;
rcm:
    if (x < 3644) goto ph;
    if (s > 454) goto zd;
    if (a < 918) goto hpx;
    goto gvj;
zb:
    if (x < 3303) return 1;
    if (s > 2217) return 0;
    if (x < 3658) return 0;
    return 1;
kdp:
    if (x < 362) goto tlr;
    goto gmc;
ktr:
    if (a > 1904) goto jp;
    if (x < 1408) goto pvc;
    if (s < 2789) goto grh;
    goto lgc;
gxc:
    if (x > 2599) return 1;
    if (m < 2068) goto dhg;
    if (m < 3120) goto vz;
    goto rzd;
hg:
    if (a > 1032) goto hcr;
    if (x > 2532) return 1;
    if (m > 1001) return 0;
    goto vkn;
cqm:
    if (x > 2686) return 1;
    return 0;
txt:
    if (s > 3799) return 1;
    return 0;
rm:
    if (a < 1221) goto rpj;
    if (s > 1945) goto qd;
    goto rkt;
zk:
    if (m > 3150) return 0;
    return 1;
jsh:
    return 0;
zx:
    if (s > 2688) return 0;
    if (m > 212) return 1;
    return 0;
bt:
    if (x > 2795) return 0;
    if (s < 912) goto klt;
    if (s > 1027) goto dxz;
    goto rkv;
lfz:
    return 1;
qsq:
    if (x > 3672) return 0;
    return 1;
hth:
    if (s < 894) return 1;
    return 0;
sx:
    if (x < 3576) goto clt;
    return 1;
xst:
    if (m < 3436) return 1;
    return 0;
fx:
    if (s > 3411) goto gtb;
    if (x < 3262) goto zt;
    return 1;
kb:
    return 1;
stz:
    if (x < 538) return 0;
    return 1;
cfl:
    if (a < 288) return 0;
    return 1;
nx:
    if (s > 3743) return 1;
    if (s > 3424) goto lm;
    if (s > 3354) goto vc;
    goto gz;
vc:
    if (m < 2127) return 0;
    return 1;
kz:
    if (a < 802) return 0;
    if (x > 3343) return 0;
    if (s < 3157) return 0;
    return 1;
cp:
    if (m < 550) goto zz;
    if (s > 1058) goto vzq;
    goto zsk;
ptn:
    if (a < 330) goto xl;
    if (m > 1216) goto rdl;
    if (s < 1500) goto dzk;
    goto hnj;
qbn:
    if (m > 1670) return 0;
    if (s < 444) return 1;
    return 0;
qv:
    if (a < 882) goto dzl;
    if (m < 1431) goto gth;
    if (a < 900) goto vk;
    goto tn;
pd:
    return 0;
fvr:
    if (s < 3633) goto jxd;
    goto zzb;
hp:
    if (x < 3528) return 0;
    if (a > 1546) return 1;
    if (x > 3734) return 1;
    return 0;
dxq:
    if (x > 3246) goto jcs;
    goto zsj;
lqj:
    if (m > 1939) goto xzk;
    return 0;
qjr:
    if (x > 955) goto bnz;
    if (a > 1343) goto hz;
    if (m < 1178) goto dxg;
    goto qqq;
dmf:
    if (m > 1992) return 1;
    if (a < 1865) return 0;
    return 1;
rzd:
    if (s < 2980) return 0;
    if (s < 3176) return 1;
    return 0;
jn:
    if (a > 2806) goto grb;
    if (a > 2170) goto fts;
    if (a < 1863) goto lf;
    goto hq;
lk:
    if (m > 3740) goto clj;
    goto fx;
lh:
    if (s > 3014) return 1;
    if (a < 1066) return 1;
    if (x > 3035) goto rf;
    goto lnp;
pdc:
    if (x < 3382) return 0;
    if (s > 2340) return 0;
    if (a < 1441) return 1;
    goto pct;
hxd:
    return 1;
qmn:
    if (s < 1644) return 0;
    return 1;
xqj:
    if (s < 3059) return 0;
    return 1;
mmj:
    if (x > 2544) goto pbq;
    if (s > 2822) goto rq;
    if (m > 2713) goto ps;
    goto tc;
mq:
    return 0;
pkh:
    if (s < 3318) goto hql;
    if (x > 2874) goto qvx;
    if (m < 2884) goto drx;
    goto zf;
cc:
    if (m < 2619) goto qbn;
    if (m > 3237) goto ls;
    if (a > 1363) return 0;
    goto gbz;
bs:
    return 1;
rq:
    return 0;
ffj:
    if (m > 1170) goto mbv;
    goto xkx;
lmz:
    if (m < 1255) return 1;
    if (s > 1599) return 1;
    return 0;
mfj:
    if (m < 3463) return 0;
    if (m > 3773) return 0;
    if (x < 1389) return 1;
    return 0;
ns:
    if (a < 309) return 1;
    if (x > 1942) return 0;
    return 1;
gx:
    if (x > 485) return 0;
    if (x > 184) return 1;
    if (a > 2713) return 0;
    return 1;
xt:
    if (x > 3687) goto pv;
    return 1;
bdr:
    if (m < 3074) return 1;
    return 0;
lb:
    return 1;
hcq:
    if (m < 3054) goto kc;
    goto vg;
zhj:
    if (m < 2308) return 0;
    return 1;
zjt:
    if (m < 811) return 0;
    return 1;
kcd:
    if (x > 3716) return 1;
    if (x < 3658) return 1;
    return 0;
xjb:
    if (x < 3023) goto nd;
    if (s < 2914) goto bk;
    if (a > 1026) goto pls;
    return 1;
pvj:
    if (s < 3618) return 1;
    return 0;
njn:
    return 1;
bzs:
    if (x < 2498) goto ss;
    if (a > 925) goto pcb;
    goto nr;
lpr:
    return 1;
qt:
    if (s < 2992) return 0;
    return 1;
brc:
    return 1;
tj:
    if (a < 1086) goto zlh;
    if (a > 1462) goto hh;
    goto kq;
mp:
    if (s > 3203) return 1;
    if (m < 2433) return 0;
    goto sm;
fch:
    if (m < 902) return 1;
    return 0;
tt:
    if (s < 3117) return 0;
    if (a < 2593) return 1;
    return 0;
mfb:
    if (s > 899) return 1;
    return 0;
ls:
    if (a > 1464) return 0;
    if (s > 707) return 1;
    if (s < 275) return 1;
    return 0;
hf:
    if (x < 3413) return 0;
    if (s > 3585) return 1;
    if (m > 1596) return 1;
    return 0;
jxd:
    if (s > 3256) goto jkv;
    if (m > 3335) goto kd;
    goto ff;
gmc:
    if (a < 826) return 1;
    return 0;
hmk:
    return 1;
fjb:
    return 0;
qqq:
    if (m > 1695) goto qz;
    if (m > 1456) goto drj;
    goto kdp;
fg:
    if (m < 1522) return 1;
    if (a > 520) goto cqb;
    if (s > 2397) goto fjb;
    return 1;
qg:
    if (a < 2533) goto cjd;
    if (s > 2230) return 0;
    if (a < 3328) goto zj;
    return 0;
dgx:
    return 0;
zcp:
    if (a > 806) return 0;
    if (m < 1282) goto nzg;
    return 1;
bf:
    if (m < 1701) return 0;
    if (s < 947) return 1;
    return 0;
vjm:
    if (s > 2448) return 0;
    if (x < 2795) return 0;
    return 1;
jvt:
    return 0;
nkt:
    if (m < 465) return 1;
    return 0;
jcs:
    if (m < 2095) goto ptn;
    if (a < 511) goto dt;
    goto znn;
pz:
    if (a > 1750) return 0;
    return 1;
sn:
    if (s < 885) return 1;
    return 0;
dmj:
    if (m > 3082) return 1;
    if (s > 3831) return 1;
    goto dfc;
jcb:
    if (s > 3713) return 1;
    goto nhc;
xx:
    if (x < 1726) return 0;
    if (s < 1933) return 0;
    return 1;
hpd:
    if (s > 1881) return 1;
    if (m < 1645) return 0;
    if (m > 1662) goto pq;
    goto pn;
xp:
    if (x < 3212) return 0;
    if (x < 3735) return 0;
    if (m < 3680) return 1;
    return 0;
fhg:
    if (x > 1483) return 1;
    if (a > 3298) return 0;
    return 1;
mx:
    if (m < 2349) goto jn;
    goto rz;
qss:
    if (m > 3082) return 0;
    if (s > 205) return 1;
    return 0;
nb:
    return 0;
mv:
    return 1;
vk:
    if (x < 2918) return 0;
    return 1;
bcn:
    if (a < 407) return 0;
    if (s < 2493) return 1;
    return 0;
pbq:
    return 1;
dl:
    if (s > 3818) return 0;
    return 1;
dmh:
    if (a < 2446) return 0;
    if (s < 1865) return 1;
    if (x < 1709) return 1;
    return 0;
ndf:
    if (x > 3135) goto bvq;
    if (m < 1374) goto sf;
    goto vch;
mbv:
    if (a > 2892) goto jh;
    if (x < 629) goto dmf;
    return 0;
cnp:
    if (s > 1241) return 0;
    return 1;
fqg:
    if (s < 3752) return 1;
    if (x > 380) return 0;
    return 1;
kgq:
    if (x < 654) return 0;
    if (m < 3379) return 0;
    return 1;
ch:
    if (m > 1843) return 1;
    return 0;
gvq:
    if (m < 2229) return 1;
    goto lxq;
gq:
    if (m > 3374) goto xp;
    if (x > 3269) return 1;
    if (a < 2230) return 0;
    goto knn;
pvg:
    if (x < 2553) goto kl;
    if (m > 3879) return 1;
    goto jhj;
vdr:
    return 1;
nk:
    if (x < 497) goto tzn;
    goto bdr;
}

int main(void)
{
    int sum = 0;
    for (int i = 0; i < N; ++i)
        if (eval(rating[i][0], rating[i][1], rating[i][2], rating[i][3]))
            sum += rating[i][0] + rating[i][1] + rating[i][2] + rating[i][3];
    printf("Part 1: %d\n", sum);  // 489392

    long long s2 = 0;
    for (int x = 1; x < 4001; ++x) {
        putc('.', stdout);
        fflush(stdout);
        for (int m = 1; m < 4001; ++m)
        for (int a = 1; a < 4001; ++a)
        for (int s = 1; s < 4001; ++s)
            if (eval(x, m, a, s))
                s2 += x + m + a + s;
    }
    printf("Part 2: %lld\n", s2);  // ?
    return 0;
}
