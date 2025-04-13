#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>  // std::find için
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
int iarrGuc[] = {180, 200, 120};
// istasyonlar(iIstasyonSayisi, sure) = struct('iSoc',0,  'iEnerji', 0, 'iGiris', 0,'iGecenSure', 0, 'iGuc', 0);
struct Arac {
    float fSoc;
    float fAnlikSoc;
    float fEnerji;
    int iGirisDakika;
    int iKalanZaman;
    int iGuc;
    int iBulunduguDakika;
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

void plotWithGnuplot(const std::vector<DataPoint>& data, const char* cpwindowTitle) 
{
    FILE* gnuplot = popen("gnuplot -persistent", "w");
    if (gnuplot) {
        char* command = new char[500];
        sprintf(command, "set term wxt title '%s'\n", cpwindowTitle);
        fprintf(gnuplot, "%s", command);
        fprintf(gnuplot, "plot '-' using 1:2 with linespoints\n");
        for (const auto& point : data) {
            fprintf(gnuplot, "%f %f\n", point.x, point.y);
        }
        fprintf(gnuplot, "e\n");
        fclose(gnuplot);
    }
}

float socDegerineGoreDusecekGucHesapla(float fSoc, float fToplamSoc, float fDusmesiGerekenGuc)
{
    float fYuzdeSoc = ((fToplamSoc - fSoc)/fToplamSoc);
    if(fYuzdeSoc == 0)
    {
        return fDusmesiGerekenGuc;
    }
    if(fYuzdeSoc < 0)
    {
        printf("Negatif deger geldi %f %f %f\n", fYuzdeSoc, fToplamSoc, fSoc);
        //std::cout << fToplamSoc <<" " << fSoc << " " << fYuzdeSoc << std::endl;
    }
        
    return (fDusmesiGerekenGuc * fYuzdeSoc);
}

int HataTablosuKatSayiHesapla(float fHataYuzdesi)
{
    if(fHataYuzdesi <= 1.0)
        return 0;

    int iDeger = 1;
    while(true)
    {
        if(fHataYuzdesi - 5.0 <= 0.0) // 5.0 degeri konfig e alincak
            return iDeger;
        else
        {
            iDeger++;
            fHataYuzdesi -= 5.0;
        }
            
    }
}

float setPointBelirle(float fTrafoBinmisGuc, float fTrafoGuc)
{
    if(fTrafoBinmisGuc>= fTrafoGuc && fTrafoBinmisGuc < fTrafoGuc*1.25)
        return fTrafoGuc;
    else if(fTrafoBinmisGuc>= fTrafoGuc* 1.25 && fTrafoBinmisGuc < fTrafoGuc*1.5)
        return fTrafoGuc*1.125;
    else if(fTrafoBinmisGuc>= fTrafoGuc* 1.5 && fTrafoBinmisGuc < fTrafoGuc*1.75)
        return fTrafoGuc*1.25;
    else
        return fTrafoGuc*1.375;
}


void kesisenGucBilgileriniHesapla(map<int, int> m1, map<int, vector<Arac>>&  mIstasyonlar, float fSebekeyeHamYuk, int iSure, float fSetPoint, float fBinmisYuk)
{
    int iIndeks = 0;
    for (auto pair : m1) 
    {
        if (pair.second != -1) iIndeks++;
    }

    auto totalSoc = 0.0;
    map<float, map<int,int>> m2;
    int iSebekelerdenBinenGuc = 0;
    int iIstasyonSayisi = 0;
    for (auto pair : m1) 
    {
        if (pair.second != -1) 
        {
            if (mIstasyonlar.find(pair.first) != mIstasyonlar.end() &&
                pair.second < mIstasyonlar[pair.first].size()) 
            {
                float fSoc = mIstasyonlar[pair.first][pair.second].fSoc;
                m2[fSoc][pair.first] = pair.second;
                //std::cout << "fsoc " << fSoc << " istasyon " << pair.first << " arac " << pair.second << std::endl;
                if (fSoc < 0) 
                    //printf("Negatif fSoc tespit edildi! %f %d %d\n", fSoc, pair.first, pair.second);

                totalSoc += fSoc;
                iSebekelerdenBinenGuc += iarrGuc[pair.first];//mIstasyonlar[pair.first][pair.second].iGuc;
                iIstasyonSayisi++;
            }
        }
    }

    float fHataOraniDuzeltilmis = ((fBinmisYuk - fSetPoint)/fSetPoint) * 100.0; // 1000.0 degeri config e alinacak 
    //printf("fBinmisYuk %f fHata Orani duzeltilmis %f fSetPoint %f fSebekeyeHamYuk %f iSebekelerdenBinenGuc %d\n", fBinmisYuk, fHataOraniDuzeltilmis, fSetPoint, fSebekeyeHamYuk, iSebekelerdenBinenGuc);
    if(fHataOraniDuzeltilmis < 0 )
    {
        printf("Negatif deger var hata oraninda %f\n", fHataOraniDuzeltilmis);
        printf("fBinmisYuk %f fSebekeyeHamYuk %f iSebekelerdenBinenGuc %d fSetPoint %f\n", fBinmisYuk, fSebekeyeHamYuk, iSebekelerdenBinenGuc, fSetPoint);
    }
        
    //fHataOraniDuzeltilmis /= iIstasyonSayisi; 
    int iKatsayi = HataTablosuKatSayiHesapla(fHataOraniDuzeltilmis*0.95); // 0.95 kismi config e alinacak
    float fCikacakDeger = fHataOraniDuzeltilmis;//*iKatsayi >= 100.0 ? 50 : fHataOraniDuzeltilmis*iKatsayi;
    for (auto outerIt = m2.rbegin(); outerIt != m2.rend(); ++outerIt) 
    {
        float outerKey = outerIt->first; // soc bilgisi
        const std::map<int, int>& innerMap = outerIt->second; // istasyon ve arac bilgisi iceren iterasyon
        
        if (!innerMap.empty()) 
        {
            auto innerIt = innerMap.begin();
            int innerKey = innerIt->first;
            int innerValue = innerIt->second;
            //std::cout << "istasyon " << innerKey << " arac " << innerValue << std::endl;

            if (mIstasyonlar.find(innerKey) != mIstasyonlar.end() &&
                innerValue <= mIstasyonlar[innerKey].size()) 
            {
                if(iKatsayi <= 0)
                    mIstasyonlar[innerKey][innerValue].iGuc = iarrGuc[innerKey];
                else
                    mIstasyonlar[innerKey][innerValue].iGuc = iarrGuc[innerKey]* (100.0 - fCikacakDeger)/100; 
                //printf("iSure %d %d.istasyon %d.arac  Guc = [%d] HataOrani =[%f] KatSayi =[%d] \n", iSure, innerKey, innerValue, mIstasyonlar[innerKey][innerValue].iGuc, fHataOraniDuzeltilmis, iKatsayi);
            }
            //printf("%d Istasyon %d Arac %d Guc\n", innerKey, innerValue, mIstasyonlar[innerKey][innerValue].iGuc);
        }
    }
    //printf("<<<<<<<<<<<<>>>>>> \n");
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
        getline(ss, a, ';');  // Üçüncü sütun
        row.fActivePower = stof(a) * 125.0;
        getline(ss, a, ';');  // Üçüncü sütun
        row.fReactivePower = stof(a) * 125.0;
        row.fTotalPower = sqrt(pow(row.fActivePower, 2) + pow(row.fReactivePower, 2));
        dataRows.push_back(row);
    }

    // Dosyayı kapat
    file.close();


    map<int, vector<int>> mIstasyonlaraGirenAraclar;
    mIstasyonlaraGirenAraclar[0] = {112,447, 702, 755, 823, 868, 900, 960, 1040, 1095, 1150, 1200, 1249};
    mIstasyonlaraGirenAraclar[1] = {542,717, 855, 900, 975, 1150, 1200, 1300, 1389};
    mIstasyonlaraGirenAraclar[2] = {304,632, 699, 900,954,1150,1276, 1342, 1408};

    map<int, vector<Arac>> mIstasyonlar;
    map<int, vector<Arac>> mIstasyonlarTemp;

    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            Arac stArac;
            stArac.iGirisDakika = mIstasyonlaraGirenAraclar[i][j];
            stArac.iBulunduguDakika = stArac.iGirisDakika; 
            stArac.fEnerji = generateRandomValFloat(60.0, 100.0);
            stArac.iGuc = iarrGuc[i];
            stArac.fSoc = generateRandomValFloat(0, 20);
            stArac.iKalanZaman = gecenSureyiHesapla(stArac.fEnerji, stArac.iGuc, stArac.fSoc);
            mIstasyonlar[i].push_back(stArac);
            mIstasyonlarTemp[i].push_back(stArac);
        }
    }

    //int iZaman = 0;
    map<int,float> vVal;
    map<int,float> mIstasyonHamGuc;
    map<int, float> mIstasyonaGoreAnlikSoc;
    map<int, float> mIstasyonaGoreAnlikSoc2;
    map<int ,map<int ,map<int, int>>> mIstasyonGirisCikisGenel;
    map<int ,map<int ,map<int, int>>> mIstasyonGirisCikisGenel2;

    for(int m = 0; m < 1440; m++)
    {
        vVal[m] = dataRows[m].fActivePower;
        mIstasyonHamGuc[m] = dataRows[m].fActivePower;
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
                if(m >= mIstasyonlarTemp[i][j].iGirisDakika &&  mIstasyonlarTemp[i][j].fSoc <= 80.0)
                {
                    mIstasyonGirisCikisGenel2[i][j][mIstasyonlarTemp[i][j].iGirisDakika] = m;  
                    mIstasyonaGoreAnlikSoc2[i] = fSocHesapla(mIstasyonlarTemp[i][j].fEnerji , mIstasyonlarTemp[i][j].iGuc , mIstasyonlarTemp[i][j].fSoc);
                    mIstasyonlarTemp[i][j].fSoc = mIstasyonaGoreAnlikSoc2[i]; 
                }
            }
        }
    }

    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            int iKaldigiSure = mIstasyonGirisCikisGenel2[i][j][mIstasyonlarTemp[i][j].iGirisDakika] - mIstasyonlarTemp[i][j].iGirisDakika;
            std::cout<< "Ham hali "<<i +1<<".istasyon " <<j + 1<< ".arac"<< " giris sure "<< mIstasyonlarTemp[i][j].iGirisDakika<<" kaldigi sure " << iKaldigiSure<< std::endl;
        }
    }


    std::map<std::tuple<int, int, int>, int> mIstasyonlarYeniGuc;
    float fSetPoint = 0.0;
    float fTrafoGuc = 800.0; // trafo gücü 1600 kVA olarak kabul edildi
    for(int m = 0; m < 1440; m++)
    {   
        map<int, int> v;
        bool bSebekeyeYukBinmis = false;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        { 
            v[i]  = -1;  
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if(m == mIstasyonlar[i][j].iBulunduguDakika && vVal[m] >= fTrafoGuc)
                {
                    v[i] = j;
                    bSebekeyeYukBinmis = true;
                }
            }
        }

        if (bSebekeyeYukBinmis == true)
        {
           fSetPoint = setPointBelirle(vVal[m], fTrafoGuc);
           //printf("Set pointler %f  binen yuk %f\n", fSetPoint, vVal[m]);
           kesisenGucBilgileriniHesapla(v, mIstasyonlar, mIstasyonHamGuc[m], m, fSetPoint, vVal[m]);
        }
        
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                //std::cout <<i <<" .istasyon"<< j << " .arac "<<"Soc degeri "<<mIstasyonlar[i][j].fSoc<<std::endl;
                if(m == mIstasyonlar[i][j].iBulunduguDakika &&  mIstasyonlar[i][j].fSoc <= 80.0)
                {
                    mIstasyonGirisCikisGenel[i][j][mIstasyonlar[i][j].iGirisDakika] = m;  
                    mIstasyonaGoreAnlikSoc[i] = fSocHesapla(mIstasyonlar[i][j].fEnerji , mIstasyonlar[i][j].iGuc, mIstasyonlar[i][j].fSoc);
                    mIstasyonlarYeniGuc[{m, i, j}] = mIstasyonlar[i][j].iGuc;
                    mIstasyonlar[i][j].fSoc = mIstasyonaGoreAnlikSoc[i]; 
                    mIstasyonlar[i][j].iBulunduguDakika += 1;
                }
            }
        }
    }

   
    for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
    {
        for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
        {
            int iKaldigiSure = mIstasyonlar[i][j].iBulunduguDakika - mIstasyonlar[i][j].iGirisDakika;
            std::cout<< i +1<<".istasyon " <<j + 1<< ".arac"<< " Guc "<< mIstasyonlar[i][j].iGuc <<  " giris sure "<< mIstasyonlar[i][j].iGirisDakika<<" gecen sure " <<iKaldigiSure  << std::endl;
        }
    }


    vector<DataPoint> d1;
    vector<DataPoint> d2;
    int iZaman = 0;
    float iVal = 0;
    DataPoint d11;
    DataPoint d12;
    for(int m = 0; m < 1440; m++)
    {
        iZaman = m;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if((mIstasyonlarTemp[i][j].iGirisDakika +  mIstasyonlarTemp[i][j].iKalanZaman)>= m  && m >= mIstasyonlarTemp[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlarTemp[i][j].iGuc;
                }
            }
        }
        d11.x = iZaman;
        d11.y = iVal;
        d1.push_back(d11);
        iVal = 0;
    }

    plotWithGnuplot(d1, "Algo Oncesi Istasyon Guc");

    iZaman = 0;
    iVal = 0.0;

    for(int m = 0; m < 1440; m++)
    {
        iZaman = m;
        for(int i = 0; i <mIstasyonlaraGirenAraclar.size(); i++)
        {
            for(int j = 0; j < mIstasyonlaraGirenAraclar[i].size(); j++)
            {
                if((mIstasyonlar[i][j].iBulunduguDakika)>= m  ||  m == mIstasyonlar[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlarYeniGuc[{m,i,j}];
                }
            }
        }
        d12.x = iZaman;
        d12.y = iVal;
        d2.push_back(d12);
        iVal = 0;
    }

    plotWithGnuplot(d2, "Algo Sonrasi Istasyon Guc");


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
                if((mIstasyonlarTemp[i][j].iGirisDakika +  mIstasyonlarTemp[i][j].iKalanZaman)>= m  && m >= mIstasyonlarTemp[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlarTemp[i][j].iGuc;
                }
            }
        }
        d11.x = iZaman;
        d11.y = iVal;
        d1.push_back(d11);       
    }

    plotWithGnuplot(d1, "Istasyona Binen yuk algo oncesi");


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
                if((mIstasyonlar[i][j].iBulunduguDakika)>= m  ||  m == mIstasyonlar[i][j].iGirisDakika)
                {
                    iVal+= mIstasyonlarYeniGuc[{m,i,j}];
                }
            }
        }
        d11.x = iZaman;
        d11.y = iVal;
        d1.push_back(d11);       
    }

    plotWithGnuplot(d1, "Istasyona Binen yuk algo sonrasi");

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