#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class Point{

private:
    int pId, cId;
    int dimensions;
    vector<double> values;

public:
    Point(int id, string line){
        dimensions = 0;
        pId = id;
        stringstream is(line);
        double val;
        while(is >> val){
            values.push_back(val);
            dimensions++;
        }
        cId = 0; 
    }

    

    int getID(){
        return pId;
    }
    
    int getDimensions(){
        return dimensions;
    }

    int getCluster(){
        return cId;
    }

    void setCluster(int num){
        cId = num;
    }

    double getVal(int possition){
        return values[possition];
    }
};

class Cluster{

private:
    int cId;
    vector<double> centroid;
    vector<Point> points;

public:
    Cluster(int cId, Point centroid){
        this->cId = cId;
        for(int i=0; i<centroid.getDimensions(); i++){
            this->centroid.push_back(centroid.getVal(i));
        }
        this->addPoint(centroid);
    }

    void addPoint(Point z){
        z.setCluster(this->cId);
        points.push_back(z);
    }

    bool removePoint(int pId){
        int size = points.size();

        for(int i = 0; i < size; i++)
        {
            if(points[i].getID() == pId)
            {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    

    int getSize(){
        return points.size();
    }
    
    int getId(){
        return cId;
    }

    Point getPoint(int possition){
        return points[possition];
    }
    
    void setCentroidByPos(int possition, double num){
        this->centroid[possition] = num;
    }

    double getCentroidByPos(int possition) {
        return centroid[possition];
    }

    
};

class KMeans{
private:
    int K, itersnum, dimensions, totalP;
    vector<Cluster> clusters;

    int getNearestClusterId(Point point){
        double sum = 0.0, min_dist;
        int closestCId;

        for(int i = 0; i < dimensions; i++)
        {
            sum += pow(clusters[0].getCentroidByPos(i) - point.getVal(i), 2.0);
        }

        min_dist = sqrt(sum);
        closestCId = clusters[0].getId();

        for(int i = 1; i < K; i++)
        {
            double dist;
            sum = 0.0;

            for(int j = 0; j < dimensions; j++)
            {
                sum += pow(clusters[i].getCentroidByPos(j) - point.getVal(j), 2.0);
            }

            dist = sqrt(sum);

            if(dist < min_dist)
            {
                min_dist = dist;
                closestCId = clusters[i].getId();
            }
        }

        return closestCId;
    }

public:
    KMeans(int K, int iterations){
        this->K = K;
        this->itersnum = iterations;
    }

    void run(vector<Point>& allPoints){

        totalP = allPoints.size();
        dimensions = allPoints[0].getDimensions();


        //Initializing Clusters
        vector<int> used_pointIds;

        for(int i=1; i<=K; i++)
        {
            while(true)
            {
                int index = rand() % totalP;

                if(find(used_pointIds.begin(), used_pointIds.end(), index) == used_pointIds.end())
                {
                    used_pointIds.push_back(index);
                    allPoints[index].setCluster(i);
                    Cluster cluster(i, allPoints[index]);
                    clusters.push_back(cluster);
                    break;
                }
            }
        }

        
        
        ofstream outfile;
        outfile.open("output.txt");
        if(outfile.is_open()){


            int iter = 1;
            while(true)
            {
                cout<<"Iterration "<<iter<<endl;
                outfile<<"Iterration "<<iter<<endl;
                
                cout<<endl;
                outfile<<endl;
            
                bool done = true;

                // Add all points to their nearest cluster
                for(int i = 0; i < totalP; i++)
                {
                    int currentCId = allPoints[i].getCluster();
                    int nearestCId = getNearestClusterId(allPoints[i]);

                    if(currentCId != nearestCId)
                    {
                        if(currentCId != 0){
                            for(int j=0; j<K; j++){
                                if(clusters[j].getId() == currentCId){
                                    clusters[j].removePoint(allPoints[i].getID());
                                }
                            }
                        }

                        for(int j=0; j<K; j++){
                            if(clusters[j].getId() == nearestCId){
                                clusters[j].addPoint(allPoints[i]);
                            }
                        }
                        allPoints[i].setCluster(nearestCId);
                        done = false;
                    }
                }

                // Recalculating the center of each cluster
                for(int i = 0; i < K; i++)
                {
                    int ClusterSize = clusters[i].getSize();

                    for(int j = 0; j < dimensions; j++)
                    {
                        double sum = 0.0;
                        if(ClusterSize > 0)
                        {
                            for(int p = 0; p < ClusterSize; p++)
                                sum += clusters[i].getPoint(p).getVal(j);
                            clusters[i].setCentroidByPos(j, sum / ClusterSize);
                        }
                    }
                }
            
                for(int i=0; i<K; i++){
                    cout<<"Cluster "<<clusters[i].getId()<<" : ";
                    outfile<<"Cluster "<<clusters[i].getId()<<" : ";
                    for(int j=0; j<clusters[i].getSize(); j++){
                        cout<<clusters[i].getPoint(j).getID()<<" ";
                        outfile<<clusters[i].getPoint(j).getID()<<" ";
                    }
                    cout<<endl;
                    outfile<<endl;
                    cout<<"Centroid : (";
                    outfile<<"Centroid : (";
                    for(int x=0; x<dimensions; x++){
                        cout<<clusters[i].getCentroidByPos(x)<<" ";     //Output to console
                        outfile<<clusters[i].getCentroidByPos(x)<<" ";  //Output to file
                    }
                    cout<<")";
                    outfile<<")";
                    cout<<endl<<endl;
                    outfile<<endl<<endl;
                }
                cout<<endl;
                outfile<<endl;
            
            
                if(done || iter >= itersnum)
                {
                        
                        break;
                }
           
                iter++;
            }
            outfile.close();
        }
        else{
            cout<<"Error writing to output.txt";
        }

    }
};

int main(int argc, char **argv){

    
    int K = 3;

    string filename = "input.txt";
    ifstream infile(filename.c_str());

    if(!infile.is_open()){
        cout<<"Error opening file."<<endl;
        return 1;
    }


    int pId = 1;
    vector<Point> allPoints;
    string line;

    while(getline(infile, line)){
        Point point(pId, line);
        allPoints.push_back(point);
        pId++;
    }
    infile.close();
    

    
    int itersnum = 100;

    KMeans kmeans(K, itersnum);
    kmeans.run(allPoints);

    return 0;
}
