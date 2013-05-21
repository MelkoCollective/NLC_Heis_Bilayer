#include "GenHam.h"

// This is the main .cpp code that generates the Hamiltonian in sparse-Matrix form
// In this case, it is the Incomplete Bilayer Heisenberg model

//----------------------------------------------------------
GENHAM::GENHAM(const int Ns, const long double  J_, const long double J2_, vector <pair<int,int> > BBond_, bool Field, double mag_)   
//create bases and determine the dimension of the Hilbert space
{
  JJ = J_; //heisenberg exchange value
  JJ2 = J2_; // J2 value (for heis bilayer)

  Bond = BBond_;

  unsigned int Dim;
  Nsite = Ns;

  Dim = 2;  //  S=1/2 models : two states
  for (int ch=1; ch<Nsite; ch++) Dim *=2;

  //BasPos holds the position of state x (in the vector Basis) in its x^th element
  BasPos.resize(Dim,-1); //initialization 
  Vdim=0;
  unsigned long temp;    //create basis

  for (unsigned long i1=0; i1<Dim; i1++) 
  {
      temp = 0;
      for (int sp =0; sp<Nsite; sp++)
          temp += (i1>>sp)&1;  //unpack bra & count the up spins

      //Specifically targe the Sz=0 sector: HAMILTONIAN MUST CONSERV Sz
      if (temp==(Nsite/2) ){ 
          Basis.push_back(i1);
          BasPos.at(i1)=Basis.size()-1;
          Vdim++;
      }

  }//Dim

  //output the total and reduced Hilbert Space dimension
  cout<<"Vdim "<<Vdim<<" "<<Dim<<endl;

}//constructor


//----------------------------------------------------------
void GENHAM::printg()
{
  int i,j;
  vector<int> tempP;
  vector<long double> tempV;

  for (i=0; i<PosHam.size(); i++){
    //cout<<PosHam[i][0]<<" * ";
    cout<<i+1<<" * ";
    for (j=0; j<=PosHam[i][0]; j++){
     cout<<"("<<PosHam[i][j]+1<<","<<ValHam[i][j]<<") ";
   }
   cout<<endl;
  }

}//print


//----------------------------------------------------------
void GENHAM::SparseHamJQ()
//This function generates a sparse-matrix representation of the Hamiltonian.

{
  int ii, jj;

  int Rsize;
  vector<long> tempBas;
  vector<long double> tempH;

  unsigned long tempi, tempj, tempod;
  int si, sj; 
  double tempD;

  // Loop through all the basis states (spin 0 sector)
  for (ii=0; ii<Basis.size(); ii++){
      tempH.clear(); 
      tempBas.clear();

      tempi = Basis.at(ii);
      tempBas.push_back(0); //first element (Row size)
      tempH.push_back(0); //make base 0

      //-----1:  Create the diagonal part of the Hamiltonian
      tempBas.push_back(BasPos.at(tempi));  

      // HdiagPart has *two* indices in the TFIM case.... do we need that?
      tempD = (*this).HdiagPart(tempi);  //tempD = address of GENHAM.Hdiagpart(i)
      tempH.push_back(tempD); 

      for (int T0=0; T0<Bond.size(); T0++){ //T0 is your square index

          // si is the first element of the T0^th bond
          si = Bond[T0].first; //the lower left bond spin is not always T0
          //if (si != T0) cout<<"Square error 2\n";
          //-----2:   first bond (Horizontal)
          tempod = tempi;
          sj = Bond[T0].second; // Second element of T0th bond
          tempod ^= (1<<si);   //flips si spin in tempod
          tempod ^= (1<<sj);   //flips sj spin in tempod
          if (BasPos.at(tempod) != -1 && BasPos.at(tempod) > ii){ //build only upper half of matrix
              tempBas.push_back(BasPos.at(tempod));
              tempD = (*this).HOFFdBondX(T0,tempi);
              tempH.push_back(tempD); 
          }

      }//si

      tempBas.at(0) = tempBas.size()-1;
      //cout<<tempBas.at(0)<<" "<<tempBas.size()<<" "<<tempH.size()<<endl;

      //bubble sort (slow) 
      long stemp;
      bool noswap = false;
      while (noswap == false){
          noswap = true; 
          for (int i2=1; i2<tempBas.size()-1; i2++){ //ignore 0 element
              if (tempBas.at(i2) > tempBas.at(i2+1) ) {
                  stemp = tempBas.at(i2);
                  tempBas.at(i2) = tempBas.at(i2+1);
                  tempBas.at(i2+1) = stemp;
                  tempD = tempH.at(i2);
                  tempH.at(i2) = tempH.at(i2+1);
                  tempH.at(i2+1) = tempD;
                  noswap = false;
              }
          }//i2
      }//while

      PosHam.push_back(tempBas);
      ValHam.push_back(tempH);

  }//ii       

}//Heisenberg

//----------------------------------------------------------
double GENHAM::HdiagPart(const long bra){

  int S0b,S1b ;  //spins (bra 
  int T0,T1;  //site
  //  int P0, P1, P2, P3; //sites for plaquette (Q)
  //  int s0p, s1p, s2p, s3p;
  double valH = 0;

  for (int Ti=0; Ti< Bond.size(); Ti++){
    //***HEISENBERG PART

    T0 = Bond[Ti].first; //first spin
    S0b = (bra>>T0)&1;  
    //if (T0 != Ti) cout<<"Square error 3\n";
    T1 = Bond[Ti].second; //second spin
    S1b = (bra>>T1)&1;  //unpack bra
    valH += JJ*(S0b-0.5)*(S1b-0.5);

  }//T0

  //cout<<bra<<" "<<valH<<endl;

  return valH;

}//HdiagPart

//----------------------------------------------------------
double GENHAM::HOFFdBondX(const int si, const long bra){

  double valH;
  int S0, S1;
  int T0, T1;

  valH = JJ*0.5; //contribution from the J part of the Hamiltonian

  return valH;

}//HOFFdPart

