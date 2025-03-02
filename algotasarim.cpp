#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <random>
using namespace std;


struct DataPoint {
    float x;
    float y;
};

struct Data {
    float fActivePower;
    float fReactivePower;
    float fTotalPower;
};

// istasyonlar(iIstasyonSayisi, sure) = struct('iSoc',0,  'iEnerji', 0, 'iGiris', 0,'iGecenSure', 0, 'iGuc', 0);
struct Arac {
    float fSoc;
    float fAnlikSoc;
    float fEnerji;
    int iGirisDakika;
    int iKalanZaman;
    int iGuc;
};

float gecenSureyiHesapla(int iEnerjiBilgisi, int iGuc, float fSocAnlik, int iSocMax = 80)
{
    float iBolunmusEnerji = ((float)iEnerjiBilgisi) / (float)iGuc;
    float iSocFarki = (-fSocAnlik + iSocMax)*0.6;
    return iBolunmusEnerji*iSocFarki;
}

float fSocHesapla(int iEnerjiBilgisi, int iGuc, float fSoc)
{
    float fDeltaKWh = (float)iGuc / 60.0;
    float fDeltaSoc = fDeltaKWh / (float)iEnerjiBilgisi;
    return (fSoc + fDeltaSoc*100.0);
}

float generateRandomValFloat(float min, float max) 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);  

    return dis(gen);  
}

int generateRandomValInt(int min, int max) 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);  

    return dis(gen);  
}

void plotWithGnuplot(const std::vector<DataPoint>& data) 
{
    FILE* gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot) {
        fprintf(gnuplot, "plot '-' using 1:2 with linespoints\n");
        for (const auto& point : data) {
            fprintf(gnuplot, "%f %f\n", point.x, point.y);
        }
        fprintf(gnuplot, "e\n");
        fclose(gnuplot);
    }
}

int main() 
{
    string filename = "household_power_consumption.txt"; 
    ifstream file(filename);

    if (!file.is_open()) 
    {
        cerr << "Dosya acilirken bir hata olustu!" << endl;
        return 1;
    }

    string line;
    vector<Data> dataRows;  // CSV satırlarını tutacak vektör

    // İlk satırı atlamak (başlık satırıysa)
    getline(file, line);  // Başlık satırını okur ama işleme almaz

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        Data row;
        string a;
        
        getline(ss, a, ';');  // İlk sütun
        getline(ss, a, ';');  // İkinci sütun
        getline(ss, a, ',');  // Üçüncü sütun
        row.fActivePower = stof(a) * 125.0;
        getline(ss, a, ',');  // Üçüncü sütun
        row.fReactivePower = stof(a) * 125.0;
        row.fTotalPower = sqrt(pow(row.fActivePower, 2) + pow(row.fReactivePower, 2));
        dataRows.push_back(row);
    }

    // Dosyayı kapat
    file.close();
    int iarrGuc[] = {180, 200, 120};

    map<int, vector<int>> mIstasyonlaraGirenAraclar;
    mIstasyonlaraGirenAraclar[0] = {112,447, 702, 755, 823, 868, 1080, 1150, 1200, 1249};
    mIstasyonlaraGirenAraclar[1] = {542,717, 855, 1131, 1171, 1216, 1389};
    mIstasyonlaraGirenAraclar[2] = {304,632, 699, 954, 1276, 1342, 1408};

    map<int, vector<Arac>> mIstasyonlar;

    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            Arac stArac;
            stArac.iGirisDakika = mIstasyonlaraGirenAraclar[i][j];
            stArac.fEnerji = generateRandomValFloat(60.0, 100.0);
            stArac.iGuc = iarrGuc[i];
            stArac.fSoc = generateRandomValFloat(0, 20);
            stArac.iKalanZaman = gecenSureyiHesapla(stArac.fEnerji, stArac.iGuc, stArac.fSoc);
            mIstasyonlar[i].push_back(stArac);
        }
    }

    int iZaman = 0;
    map<int,int> vVal;
    map<int, vector<Arac>> mIstasyonlarTemp = mIstasyonlar;
    map<int, float> mIstasyonaGoreAnlikSocDegisim;
    map<int, float> mIstasyonaGoreAnlikSocDegisim2;
    map<int ,map<int ,map<int, int>>> mIstasyonGirisCikisGenel;
    map<int ,map<int ,map<int, int>>> mIstasyonGirisCikisGenel2;

    for(int m = 0; m < 1440; m++)
    {
        vVal[m] =dataRows[m].fActivePower;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if((mIstasyonlar[i][j].iGirisDakika +  mIstasyonlar[i][j].iKalanZaman)>= m  && m >= mIstasyonlar[i][j].iGirisDakika)
                {
                    vVal[m] += mIstasyonlar[i][j].iGuc;
                }
            }
        }
    }


    for(int m = 0; m < 1440; m++)
    {
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                //mIstasyonaGoreAnlikSocDegisim2[i] =  mIstasyonlarTemp[i][j].fSoc;
                if(m >= mIstasyonlarTemp[i][j].iGirisDakika &&  mIstasyonlarTemp[i][j].fSoc <= 80.0)
                {
                    mIstasyonGirisCikisGenel2[i][j][mIstasyonlarTemp[i][j].iGirisDakika] = m;  
                    mIstasyonaGoreAnlikSocDegisim2[i] = fSocHesapla(mIstasyonlarTemp[i][j].fEnerji , mIstasyonlarTemp[i][j].iGuc , mIstasyonlarTemp[i][j].fSoc);
                    mIstasyonlarTemp[i][j].fSoc = mIstasyonaGoreAnlikSocDegisim2[i]; 
                }
            }
        }
    }

    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            std::cout<< "Ham hali "<<i +1<<".istasyon " <<j + 1<< ".arac"<< " giris sure "<< mIstasyonlarTemp[i][j].iGirisDakika<<" cikis sure " <<mIstasyonGirisCikisGenel2[i][j][mIstasyonlarTemp[i][j].iGirisDakika] << std::endl;
        }
    }


    for(int m = 0; m < 1440; m++)
    {
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if (m >= mIstasyonlar[i][j].iGirisDakika &&  mIstasyonlar[i][j].fSoc  <= 80.0 && vVal[m] > 800.0)
                { 
                    mIstasyonGirisCikisGenel[i][j][mIstasyonlar[i][j].iGirisDakika] = m;
                    mIstasyonaGoreAnlikSocDegisim[i] = fSocHesapla(mIstasyonlar[i][j].fEnerji ,  mIstasyonlar[i][j].iGuc * 0.75 , mIstasyonlar[i][j].fSoc );
                    mIstasyonlar[i][j].fSoc = mIstasyonaGoreAnlikSocDegisim[i]; 
                }
                else if(m >= mIstasyonlar[i][j].iGirisDakika &&  mIstasyonlar[i][j].fSoc <= 80.0)
                {
                    mIstasyonGirisCikisGenel[i][j][mIstasyonlar[i][j].iGirisDakika] = m;  
                    mIstasyonaGoreAnlikSocDegisim[i] = fSocHesapla(mIstasyonlar[i][j].fEnerji , mIstasyonlar[i][j].iGuc, mIstasyonlar[i][j].fSoc);
                    mIstasyonlar[i][j].fSoc = mIstasyonaGoreAnlikSocDegisim[i]; 
                }
            }
        }
    }

    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            std::cout<< i +1<<".istasyon " <<j + 1<< ".arac"<< " giris sure "<< mIstasyonlar[i][j].iGirisDakika<<" cikis sure " <<mIstasyonGirisCikisGenel[i][j][mIstasyonlar[i][j].iGirisDakika] << std::endl;
        }
    }
    


    /* //istasyonlar icin
    vector<DataPoint> d1;
    int iZaman = 0;
    float iVal = 0;
    DataPoint d11;
    for(int m = 0; m < 1440; m++)
    {
        iZaman = m;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if((mIstasyonlar[i][j].iGirisDakika +  mIstasyonlar[i][j].iKalanZaman)>= m  && m >= mIstasyonlar[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlar[i][j].iGuc;
                }
            }
        }
        d11.x = iZaman;
        d11.y = iVal;
        d1.push_back(d11);
        iVal = 0;
    }

    plotWithGnuplot(d1);
    */
    
/* // sebekeye yuk binmis hali
    d1.clear();
    iZaman = 0;
    iVal = 0;
    d11 = {};
    for(int m = 0; m < 1440; m++)
    {
        iVal = dataRows[m].fActivePower;
        iZaman = m;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if((mIstasyonlar[i][j].iGirisDakika +  mIstasyonlar[i][j].iKalanZaman)>= m  && m >= mIstasyonlar[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlar[i][j].iGuc;
                }
            }
        }
        d11.x = iZaman;
        d11.y = iVal;
        d1.push_back(d11);       
    }

    plotWithGnuplot(d1);
*/
    return 0;
}