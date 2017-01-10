#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "levmar.h"


static double data[] = {
303.35	,	82.806	,
302.98	,	82.421	,
303.01	,	82.417	,
282.68	,	80.628	,
282.84	,	80.853	,
282.84	,	80.804	,
262.59	,	78.323	,
262.7	,	78.49	,
262.7	,	78.512	,
242.43	,	75.863	,
242.52	,	76.08	,
242.51	,	75.941	,
222.29	,	73.158	,
222.37	,	73.276	,
222.36	,	73.235	,
202.13	,	69.51	,
202.18	,	69.563	,
202.18	,	69.506	,
181.94	,	65.727	,
181.98	,	65.753	,
181.98	,	65.66	,
161.75	,	61.112	,
161.76	,	61.098	,
161.76	,	61.205	,
141.55	,	55.828	,
141.55	,	55.868	,
141.55	,	55.869	,
121.36	,	49.661	,
121.35	,	49.699	,
121.35	,	49.735	,
101.18	,	42.518	,
101.17	,	42.577	,
101.16	,	42.454	,
96.13	,	40.19	,
96.12	,	40.191	,
96.12	,	40.187	,
86.29	,	36.246	,
86.28	,	36.288	,
86.28	,	36.264	,
77.57	,	32.478	,
77.56	,	32.517	,
77.56	,	32.481	,
69.7	,	28.524	,
69.69	,	28.506	,
69.69	,	28.494	,
62.61	,	24.941	,
62.6	,	24.908	,
62.6	,	24.899	,
56.26	,	21.481	,
56.25	,	21.448	,
56.25	,	21.445	,
50.54	,	18.15	,
50.53	,	18.119	,
50.53	,	18.111	,
45.41	,	15.237	,
45.4	,	15.21	,
45.4	,	15.194	,
40.81	,	12.563	,
40.8	,	12.535	,
40.8	,	12.534	,
36.67	,	10.269	,
36.66	,	10.247	,
36.66	,	10.244	,
32.95	,	8.282	,
32.93	,	8.262	,
32.94	,	8.259	,
29.61	,	6.577	,
29.59	,	6.569	,
29.59	,	6.574	,
26.6	,	5.168	,
26.59	,	5.183	,
26.59	,	5.18	,
23.91	,	4.082	,
23.89	,	4.036	,
23.89	,	4.025	,
21.47	,	3.129	,
21.47	,	3.106	,
21.46	,	3.119	,
19.3	,	2.395	,
19.28	,	2.389	,
19.28	,	2.393	,
17.34	,	1.836	,
17.32	,	1.825	,
17.32	,	1.825	,
15.58	,	1.401	,
15.57	,	1.393	,
15.57	,	1.394	,
14.01	,	1.066	,
13.99	,	1.063	,
13.99	,	1.06	,
12.58	,	0.811	,
12.57	,	0.806	,
12.57	,	0.808	,
11.3	,	0.616	,
11.3	,	0.613	,
11.29	,	0.614	,
10.15	,	0.469	,
10.15	,	0.467	,
10.15	,	0.466	,
9.11	,	0.354	,
9.11	,	0.353	,
9.11	,	0.354	,
8.19	,	0.27	,
8.19	,	0.269	,
8.19	,	0.269	,
7.35	,	0.202	,
7.35	,	0.202	,
7.35	,	0.201	,
6.62	,	0.155	,
6.61	,	0.154	,
6.61	,	0.155	,
5.94	,	0.113	,
5.94	,	0.113	,
5.95	,	0.113	,
5.33	,	0.085	,
5.33	,	0.084	,
5.33	,	0.084	,
4.79	,	0.063	,
4.79	,	0.063	,
4.79	,	0.062	,
4.3	,	0.046	,
4.3	,	0.045	,
4.3	,	0.046	,
3.86	,	0.034	,
3.86	,	0.032	,
3.86	,	0.033	,
3.47	,	0.024	,
3.47	,	0.024	,
3.47	,	0.024	,
3.12	,	0.017	,
3.11	,	0.017	,
3.11	,	0.017	,
2.8	,	0.012	,
2.8	,	0.012	,
2.8	,	0.012	,
2.52	,	0.008	,
2.51	,	0.008	,
2.51	,	0.008	,
2.26	,	0.006	,
2.26	,	0.006	,
2.26	,	0.006	,
2.03	,	0.004	,
2.03	,	0.004	,
2.03	,	0.004	,
222.34	,	72.616	,
222.42	,	72.614	,
222.42	,	72.737	,
202.16	,	69.073	,
202.21	,	69.047	,
202.21	,	69.145	,
181.97	,	65.322	,
182.0	,	65.313	,
181.99	,	65.368	,
161.78	,	60.781	,
161.79	,	60.867	,
161.78	,	60.866	,
141.6	,	55.407	,
141.57	,	55.583	,
141.57	,	55.543	,
121.41	,	49.547	,
121.36	,	49.538	,
121.35	,	49.535	,
101.22	,	42.322	,
101.16	,	42.272	,
101.16	,	42.357	,
96.09	,	40.258	,
96.1	,	40.232	,
96.1	,	40.252	,
86.31	,	36.143	,
86.27	,	36.187	,
86.27	,	36.383	,
77.58	,	32.333	,
77.55	,	32.277	,
77.56	,	32.324	,
69.7	,	28.384	,
69.68	,	28.365	,
69.68	,	28.384	,
62.61	,	24.828	,
62.58	,	24.799	,
62.59	,	24.775	,
56.25	,	21.377	,
56.23	,	21.361	,
56.23	,	21.361	,
50.53	,	18.067	,
50.51	,	18.037	,
50.51	,	18.028	,
45.4	,	15.165	,
45.38	,	15.131	,
45.38	,	15.138	,
40.79	,	12.5	,
40.77	,	12.477	,
40.77	,	12.48	,
36.65	,	10.192	,
36.63	,	10.202	,
36.64	,	10.186	,
32.93	,	8.199	,
32.91	,	8.223	,
32.91	,	8.22	,
29.59	,	6.426	,
29.56	,	6.536	,
29.57	,	6.537	,
26.59	,	5.17	,
26.57	,	5.134	,
26.57	,	5.143	,
23.88	,	4.019	,
23.87	,	3.981	,
23.87	,	4.016	,
21.46	,	3.101	,
21.44	,	3.099	,
21.45	,	3.089	,
19.27	,	2.374	,
19.26	,	2.369	,
19.26	,	2.368	,
17.32	,	1.817	,
17.31	,	1.811	,
17.31	,	1.811	,
15.57	,	1.382	,
15.56	,	1.38	,
15.56	,	1.379	,
13.99	,	1.048	,
13.98	,	1.046	,
13.98	,	1.046	,
12.57	,	0.795	,
12.56	,	0.793	,
12.56	,	0.793	,
11.28	,	0.601	,
11.28	,	0.599	,
11.28	,	0.6	,
10.13	,	0.453	,
10.13	,	0.45	,
10.13	,	0.451	,
9.1	,	0.344	,
9.1	,	0.341	,
9.1	,	0.341	,
8.18	,	0.259	,
8.17	,	0.257	,
8.18	,	0.257	,
7.35	,	0.193	,
7.34	,	0.192	,
7.35	,	0.193	,
6.61	,	0.146	,
6.6	,	0.146	,
6.6	,	0.146	,
5.93	,	0.108	,
5.93	,	0.106	,
5.93	,	0.105	,
5.33	,	0.079	,
5.32	,	0.079	,
5.33	,	0.08	,
4.79	,	0.059	,
4.78	,	0.06	,
4.79	,	0.059	,
4.3	,	0.043	,
4.3	,	0.043	,
4.3	,	0.043	,
3.86	,	0.031	,
3.86	,	0.031	,
3.86	,	0.031	,
3.47	,	0.023	,
3.47	,	0.022	,
3.47	,	0.022	,
3.12	,	0.016	,
3.11	,	0.016	,
3.11	,	0.017	,
2.8	,	0.012	,
2.79	,	0.012	,
2.8	,	0.012	,
2.52	,	0.008	,
2.51	,	0.008	,
2.51	,	0.008	,
2.26	,	0.006	,
2.26	,	0.006	,
2.26	,	0.006	,
2.03	,	0.004	,
2.03	,	0.004	,
2.03	,	0.004	,
10.09	,	0.45591	,
10.07	,	0.45395	,
10.07	,	0.45443	,
9.26	,	0.36359	,
9.25	,	0.36304	,
9.24	,	0.36349	,
8.5	,	0.29281	,
8.49	,	0.29146	,
8.49	,	0.29265	,
7.79	,	0.23436	,
7.78	,	0.23406	,
7.79	,	0.23438	,
7.14	,	0.18707	,
7.14	,	0.18666	,
7.14	,	0.18692	,
6.57	,	0.14849	,
6.56	,	0.14803	,
6.56	,	0.14787	,
6.03	,	0.11806	,
6.02	,	0.11794	,
6.02	,	0.11805	,
5.53	,	0.09294	,
5.53	,	0.09274	,
5.53	,	0.09303	,
5.08	,	0.07316	,
5.07	,	0.07278	,
5.07	,	0.07266	,
4.66	,	0.05683	,
4.66	,	0.05657	,
4.66	,	0.05665	,
4.28	,	0.04405	,
4.28	,	0.04384	,
4.27	,	0.04392	,
3.93	,	0.0341	,
3.93	,	0.034	,
3.93	,	0.03394	,
3.6	,	0.02581	,
3.6	,	0.0257	,
3.6	,	0.02573	,
3.31	,	0.01982	,
3.31	,	0.01979	,
3.31	,	0.01967	,
3.04	,	0.01502	,
3.04	,	0.01492	,
3.04	,	0.01486	,
2.79	,	0.01122	,
2.79	,	0.01118	,
2.79	,	0.01119	,
2.56	,	0.00827	,
2.56	,	0.00817	,
2.56	,	0.0082	,
2.35	,	0.00625	,
2.35	,	0.00618	,
2.35	,	0.00626	,
2.14	,	0.00461	,
2.15	,	0.00462	,
2.15	,	0.00464	,
1.97	,	0.00336	,
1.97	,	0.00339	,
1.97	,	0.00341	,
1.81	,	0.00253	,
1.81	,	0.00252	,
1.81	,	0.0025	,
1.67	,	0.00192	,
1.67	,	0.0019	,
1.67	,	0.00191	,
1.53	,	0.00144	,
1.53	,	0.00143	,
1.53	,	0.00142	,
1.41	,	0.00109	,
1.41	,	0.00107	,
1.41	,	0.00108	,
1.29	,	0.00083	,
1.29	,	0.00083	,
1.29	,	0.00082	,
1.19	,	0.00064	,
1.19	,	0.00064	,
1.19	,	0.00064	,
1.09	,	0.0005	,
1.09	,	0.0005	,
1.09	,	0.0005	,
1.0	,	0.0004	,
1.0	,	0.0004	,
1.0	,	0.0004	,
0.92	,	0.00033	,
0.92	,	0.00032	,
0.92	,	0.00033	,
0.85	,	0.00026	,
0.85	,	0.00027	,
0.85	,	0.00027	,
0.78	,	0.00023	,
0.78	,	0.00023	,
0.78	,	0.00022	,
0.72	,	0.00019	,
0.72	,	0.00019	,
0.71	,	0.00019	,
0.66	,	0.00017	,
0.66	,	0.00017	,
0.66	,	0.00017	,
0.6	,	0.00015	,
0.6	,	0.00014	,
0.6	,	0.00015	,
0.56	,	0.00014	,
0.56	,	0.00014	,
0.56	,	0.00013	,
0.51	,	0.00012	,
0.51	,	0.00011	,
0.51	,	0.00012	,
0.47	,	0.00011	,
0.47	,	0.00011	,
0.47	,	0.00011	,
0.44	,	0.0001	,
0.44	,	9e-005	,
0.44	,	0.0001	,
0.41	,	8e-005	,
0.41	,	9e-005	,
0.41	,	9e-005	,
0.38	,	9e-005	,
0.38	,	9e-005	,
0.38	,	9e-005	,
302.2	,	82.01324	,
302.27	,	82.05352	,
302.3	,	81.86941	,
10.09	,	0.45763	,
10.07	,	0.4566	,
10.07	,	0.45581	,
9.26	,	0.36464	,
9.24	,	0.36443	,
9.24	,	0.36403	,
8.49	,	0.29256	,
8.48	,	0.29218	,
8.48	,	0.29181	,
7.8	,	0.23427	,
7.78	,	0.23442	,
7.78	,	0.23442	,
7.15	,	0.18764	,
7.14	,	0.18725	,
7.14	,	0.18767	,
6.57	,	0.14821	,
6.56	,	0.14765	,
6.55	,	0.14769	,
6.55	,	0.14796	,
6.56	,	0.14799	,
6.56	,	0.14824	,
5.54	,	0.09234	,
5.52	,	0.09224	,
5.52	,	0.09229	,
5.08	,	0.07269	,
5.08	,	0.07261	,
5.08	,	0.07246	,
4.67	,	0.05651	,
4.66	,	0.05633	,
4.66	,	0.05635	,
4.28	,	0.04349	,
4.28	,	0.04343	,
4.28	,	0.04333	,
3.93	,	0.03368	,
3.93	,	0.03345	,
3.93	,	0.03344	,
3.59	,	0.02513	,
3.59	,	0.02511	,
3.6	,	0.02514	,
3.32	,	0.01942	,
3.31	,	0.01923	,
3.31	,	0.0192	,
3.04	,	0.01456	,
3.04	,	0.01446	,
3.04	,	0.01445	,
2.79	,	0.01085	,
2.79	,	0.01074	,
2.79	,	0.0108	,
2.56	,	0.00791	,
2.56	,	0.00786	,
2.56	,	0.00784	,
2.35	,	0.00597	,
2.35	,	0.00593	,
2.35	,	0.00596	,
2.15	,	0.00443	,
2.15	,	0.00439	,
2.15	,	0.00438	,
1.97	,	0.00323	,
1.97	,	0.00323	,
1.97	,	0.00324	,
1.81	,	0.00241	,
1.81	,	0.0024	,
1.81	,	0.00241	,
1.67	,	0.00184	,
1.67	,	0.00183	,
1.67	,	0.00178	,
1.53	,	0.00138	,
1.53	,	0.00137	,
1.53	,	0.00134	,
1.41	,	0.00105	,
1.41	,	0.00104	,
1.41	,	0.00106	,
1.29	,	0.0008	,
1.29	,	0.00087	,
1.29	,	0.00081	,
0.92	,	0.00035	,
0.92	,	0.00034	,
0.92	,	0.00035	,
0.85	,	0.00028	,
0.85	,	0.00027	,
0.85	,	0.0003	,
0.78	,	0.00024	,
0.78	,	0.00024	,
0.78	,	0.00025	,
0.72	,	0.0002	,
0.72	,	0.00022	,
0.66	,	0.00018	,
0.66	,	0.00017	,
0.66	,	0.00018	,
0.6	,	0.00016	,
0.6	,	0.00013	,
0.6	,	0.00015	,
0.56	,	0.00017	,
0.56	,	0.00016	,
0.51	,	0.00014	,
0.51	,	0.00013	,
0.51	,	0.00014	,
0.47	,	0.00012	,
0.47	,	0.00011	,
0.47	,	0.00012	,
0.44	,	0.0001	,
0.44	,	0.00011	,
0.4	,	8e-005	,
0.4	,	9e-005	,
0.4	,	0.0001	,
0.38	,	0.0001	,
0.38	,	0.0001	,
0.38	,	0.0001	,
279.29	,	79.616	,
279.0	,	79.456	,
279.01	,	79.398	,
295.61	,	80.64	,
295.63	,	80.431	,
295.62	,	80.384	,
312.14	,	81.615	,
312.16	,	81.436	,
312.15	,	81.415	,
329.06	,	83.359	,
328.62	,	83.311	,
328.65	,	83.262	,
345.04	,	84.306	,
345.04	,	84.099	,
278.75	,	79.087	,
278.38	,	78.936	,
278.39	,	78.859	,
301.62	,	80.588	,
301.65	,	80.457	,
301.64	,	80.326	,
324.89	,	81.951	,
324.91	,	81.842	,
324.9	,	81.837	,
347.49	,	83.12	,
347.52	,	82.73	,
347.51	,	82.982	,
369.34	,	84.964	,
369.48	,	84.476	,
369.45	,	84.613	,
392.44	,	86.112	,
392.47	,	85.806	,
392.43	,	85.849,
-1, -1};


static double *T;
static double *Cp;

#define CONST_R 8.3144598

void jacf(double *param, double *J, int m, int n, void *adata)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m / 2; j++) {
			double alpha = param[j];
			double theta = param[j + m / 2];
			double x = theta / T[i];
			double ex = exp(x);
			if (1 | x < 500) {
				J[j] = 3*ex*x*x / pow(ex - 1, 2) * CONST_R;
				J[j + m / 2] = 3*alpha / (T[i] * pow(ex - 1, 2)) * x * ex * ( 
					(2 + x) - 2*x*ex / (ex - 1)
				) * CONST_R;
			} else {
				J[j] = 3*x*x / (ex) * CONST_R;
				J[j + m / 2] = 3*alpha*x / (T[i] * ex) * (2 - x) * CONST_R;
			}
		}
		J = J + m;
	}
}


void func(double *param, double *res, int m, int n, void *adata)
{
	for (int i = 0; i < n; i++) {
		double Cpcalc = 0.0;
		for (int j = 0; j < m / 2; j++) {
			double alpha = param[j];
			double theta = param[j + m / 2];
			double x = theta / T[i];
			double ex = exp(x);

			if (x < 500) {
				Cpcalc += 3*alpha*ex*x*x / pow(ex - 1, 2);
			} else {
				Cpcalc += 3*alpha*x*x / ex;
			}
		}
		res[i] = (Cpcalc * CONST_R - Cp[i]);
	}
}


int main()
{
	int n = 0, m = 4;
	while (data[n] != -1) n = n + 2;
	n = n / 2;
	printf("Number of points: %d\n", n);

	T = calloc(n, sizeof(double));
	Cp = calloc(n, sizeof(double));
	for (int i = 0, j = 0; i < n; i++) {
		T[i] = data[j++];
		Cp[i] = data[j++];
	}



	double opts[LM_OPTS_SZ] = {LM_INIT_MU, 1e-15, 1e-15, 1e-20, LM_DIFF_DELTA};
	double info[LM_INFO_SZ];
	double *beta = (double *) calloc(m, sizeof(double));
	double *covar = (double *) calloc(m * m, sizeof(double));

	for (int i = 0; i < m / 2; i++) {
		beta[i] = 0.1;
		beta[i + m/2] = 1;
	}

	for (int i = 0; i < m; i++) {
		printf("%g", beta[i]);
	}



	dlevmar_der(
		func, jacf, beta, NULL, m, n,
		500, // Itmax
		opts, info, NULL, covar,
		NULL);

	double *J = calloc(n*m, sizeof(double));
	double *Jptr = J;
	jacf(beta, J, m, n, NULL);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%10.3g ", *Jptr++);
		}
		printf("\n");
	}




	/* Type the result */
	printf("Result:\n");
	printf("%10s %10s\n", "beta", "s(beta)");
	for (int i = 0; i < m; i++) {
		printf("%10g %10g\n", beta[i], sqrt((covar[i*m + i])));
	}


	return 0;
}
