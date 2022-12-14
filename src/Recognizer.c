#include "Dataset.h"
#include "Recognizer.h"
#include <string.h>
#include <stdlib.h>
// A compléter


kNN *recNearestNeighbors(Sketch *sk, Dataset *ds, int k, double (*distance)(Sketch *, Sketch *))
{
	kNN *KitsuNeNinetails = malloc(sizeof(kNN));
	if(!KitsuNeNinetails)
		return NULL;
	
	int *neighbors = malloc(k*sizeof(int));
	if (!neighbors)
	{
		free(KitsuNeNinetails);
		return NULL;
	}
	
	double *distances = malloc(k*sizeof(double));
	if (!distances)
	{
		free(KitsuNeNinetails);
		free(neighbors);
		return NULL;
	}

	double allDistances[dsGetNbSketches(ds)];

	for(int i=0; i < dsGetNbSketches(ds); i++)
		allDistances[i] = distance(sk,dsGetSketch(i));// il manque des argument a dsGetSketch                 // je suis plus sûr syntaxe pour pointeur fct

	int actualNeighbor = 0;
	int lastNeighbor   = 0;

	for(int i=0; i < k; i++)
	{
		for(int j=0; j < dsGetNbSketches(ds); j++)
			if(j != lastNeighbor && allDistances[j] < allDistances[actualNeighbor])
				actualNeighbor = j;

		lastNeighbor = actualNeighbor;
		neighbors[i] = actualNeighbor;
		distances[i]  = allDistances[actualNeighbor];
	}

	KitsuNeNinetails->k         = k;
	KitsuNeNinetails->Dataset   = ds;
	KitsuNeNinetails->neighbors = neighbors;
	KitsuNeNinetails->distance  = distance;

	return KitsuNeNinetails;
}

void recFreekNN(kNN *knn)
{
	free(knn->neighbors);
	free(knn->distances);
	free(knn);
	return;
}

char *recGetMajorityLabel(kNN *knn)
{
	int n     = dsGetNbLabelNames(knn->dataset);
	int label = 0;
	int numbers[n];
	float totalDistances[n];
	for(int i=0; i < n; i++)
		{
			numbers[i]        = 0;
			totalDistances[i] = 0;
		}

	for(int i=0; i < knn->k; i++)
		{
			label = dsGetLabel(knn->neighbors[i]);
			numbers[label]++;
			totalDistances[label] += knn->distances[i];
		}

	int majority = 0;
	for(int i=1; i < n; i++)
	{
		if(numbers[i]>numbers[majority])
			majority = i;
		else if(numbers[i]==numbers[majority])
			if (totalDistances[i] < totalDistances[majority]) // je gère les égalités en comparant les sommes de toutes distances => minimiser somme erreurs (=?meilleure solution)
				majority = i;
	}

	return dsGetLabelName(knn->dataset,majority);

}

float recEvalkNN(Dataset *referenceset, Dataset *testset, int k, double (*distance)(Sketch *, Sketch *),FILE *out)
{
	int n                 = dsGetNbSketches(testset);
	int correctLabelFound = 0;
	char *label;
	char *correctLabel;
	
	for(int i=0; i < n; i++)
	{
		kNN *KitsuNeNinetails = recNearestNeighbors(dsGetSketch(testset, dataset, k, distance));   // plus sûr pour pointeur fct
		label = recGetMajorityLabel(KitsuNeNinetails);
		correctLabel = dsGetLabelName(testset,dsGetLabel(testset,i));
		if(strcmp(label,correctLabel)==0)
			correctLabelFound++;
		recFreekNN(KitsuNeNinetails);
	}
	float accuracy = correctLabelFound / n;
	return accuracy;		
}
