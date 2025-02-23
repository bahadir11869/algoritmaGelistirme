clear; clc;
% Tanimlamalar ve setlemeler
sDosyaAdi = 'household_power_consumption.txt';  
cAyrac = ';';

iSocMax = 80;

iIlkIstasyonAracGirisZamani = [112 447 702 755 823 868 1080 1150 1200 1249];
iIkinciIstasyonAracGirisZamani = [542 717 855 1131 1171 1216 1389];
iUcuncuIstasyonAracGirisZamani = [304 632 699 785 954 1276 1342 1408];
iIstasyonAracGirisZamanlari =[{iIlkIstasyonAracGirisZamani} {iIkinciIstasyonAracGirisZamani} {iUcuncuIstasyonAracGirisZamani}];

arrAracSayilari = [length(iIstasyonAracGirisZamanlari{1}) length(iIstasyonAracGirisZamanlari{2}) length(iIstasyonAracGirisZamanlari{3})]; % arac sayisi istasyona giris sayisina bagli olarak degisken

sure = 1440; % 1 gunun dakika cinsinden degeri

active_power_data = zeros(1, sure);
reactive_power_data = zeros(1, sure);
total_power_data = zeros(1, sure);

iIstasyonSayisi = length(arrAracSayilari);

isebekeKatsayisi = 125;

arrIstasyonGucBilgileri = [180 200 120]; % arrIstasyonGucBilgileri istasyona giris sayisina bagli olarak degisken

iAtlanacakGun = 0; % okunulan dosyada okunacak gunun kac gunluk atlanilacaginin katsayisi

if iAtlanacakGun ~= 0
    iBaslangicGun = length(active_power_data) * iAtlanacakGun;
    iBitisGun = length(active_power_data) * (iAtlanacakGun + 1) - 1;

else
    iBaslangicGun = 1;
    iBitisGun = length(active_power_data);
end

iGelenAracEnerjiDegerAraligi = [60 100];
iGelenAracSocDegerAraligi = [0 20];

% Dosyanin okundugu yer
data = readtable(sDosyaAdi, 'Delimiter', cAyrac);
min = 1:1:sure; % 1 günlük zaman vektörü (saniye cinsinden)

% dosyadan okunan verinin eklendigi yer
iIstasyonIndex = 1;
for i = iBaslangicGun: iBitisGun
    active_power_data(iIstasyonIndex) = data{i,3} * isebekeKatsayisi;
    reactive_power_data(iIstasyonIndex) = data{i,4} *isebekeKatsayisi;
    total_power_data(iIstasyonIndex) = sqrt(power(active_power_data(i), 2) + power(reactive_power_data(i), 2));
    iIstasyonIndex = iIstasyonIndex + 1;
end

% sarj istasyonlarinan gelen bilgilerle struct doldurma islemi
istasyonlar(iIstasyonSayisi, sure) = struct('iSoc',0,  'iEnerji', 0, 'iGiris', 0,'iGecenSure', 0, 'iGuc', 0);
iIstasyonIndex = 1;
iAracIndex = 1;

while iAracIndex <= arrAracSayilari(iIstasyonIndex)
    istasyonlar(iIstasyonIndex,iAracIndex).iSoc = randi(iGelenAracSocDegerAraligi);
    istasyonlar(iIstasyonIndex,iAracIndex).iEnerji = randi(iGelenAracEnerjiDegerAraligi);
    istasyonlar(iIstasyonIndex,iAracIndex).iGiris = iIstasyonAracGirisZamanlari{iIstasyonIndex}(iAracIndex);
    istasyonlar(iIstasyonIndex,iAracIndex).iGuc = arrIstasyonGucBilgileri(iIstasyonIndex);
    istasyonlar(iIstasyonIndex,iAracIndex).iGecenSure = gecenSureyiHesapla(istasyonlar(iIstasyonIndex,iAracIndex).iEnerji, arrIstasyonGucBilgileri(iIstasyonIndex), iSocMax, istasyonlar(iIstasyonIndex,iAracIndex).iSoc);
    
    if iAracIndex == arrAracSayilari(iIstasyonIndex)
        iAracIndex = 1;
        iIstasyonIndex = iIstasyonIndex + 1;
       
        if iIstasyonIndex > length(arrAracSayilari)
            break; 
        end
    else
        iAracIndex = iAracIndex + 1;
    end
end

istasyonlarSureyeGoreDolmusHali(3, 1440) = struct('iSoc',0,  'iEnerji', 0, 'iGiris', 0,'iGecenSure', 0, 'iGuc', 0);
arac_graf = zeros(1, sure);
active_power_data_temp = active_power_data;
for i = 1: length(active_power_data)
    for iIstasyonIndex = 1 : iIstasyonSayisi
       for iAracIndex = 1 : arrAracSayilari(iIstasyonIndex)
            if( istasyonlar(iIstasyonIndex,iAracIndex).iGiris == i)
                for l=0:istasyonlar(iIstasyonIndex,iAracIndex).iGecenSure
                    istasyonlarSureyeGoreDolmusHali(iIstasyonIndex,i + l).iEnerji =  istasyonlar(iIstasyonIndex,iAracIndex).iEnerji;
                    istasyonlarSureyeGoreDolmusHali(iIstasyonIndex,i + l).iSoc =  istasyonlar(iIstasyonIndex,iAracIndex).iSoc;
                    istasyonlarSureyeGoreDolmusHali(iIstasyonIndex,i + l).iGecenSure =  istasyonlar(iIstasyonIndex,iAracIndex).iGecenSure;
                    istasyonlarSureyeGoreDolmusHali(iIstasyonIndex,i + l).iGiris =  istasyonlar(iIstasyonIndex,iAracIndex).iGiris;
                     istasyonlarSureyeGoreDolmusHali(iIstasyonIndex,i + l).iGuc =   istasyonlar(iIstasyonIndex,iAracIndex).iGuc;

                     arac_graf(i + l) = arac_graf(i + l) + istasyonlar(iIstasyonIndex,iAracIndex).iGuc;
                end 
            end         
       end
    end
end

for i = 1: length(active_power_data)
    iGucValues = [istasyonlarSureyeGoreDolmusHali(:,i).iGuc;];
    active_power_data(i) = active_power_data(i) + sum(iGucValues);
end

total_power_data_son = sqrt(active_power_data.^2 + reactive_power_data.^2);
subplot(3,1,1)
plot(min, total_power_data);
title('Baslangic Sebeke gucu', 'FontSize', 14, 'FontWeight', 'bold');
xlabel('Time (Hour:Minute)', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Power Demand (kW)', 'FontSize', 12, 'FontWeight', 'bold');

subplot(3,1,2)
plot(min, total_power_data_son);
title('Toplam Sebeke gucu', 'FontSize', 14, 'FontWeight', 'bold');
xlabel('Time (Hour:Minute)', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Power Demand (kW)', 'FontSize', 12, 'FontWeight', 'bold');

subplot(3,1,3)
plot(min, arac_graf);
title('Sarj Istasyonlari Hizli Sarj Zamanlari Toplam Guc', 'FontSize', 14, 'FontWeight', 'bold');
xlabel('Time (Hour:Minute)', 'FontSize', 12, 'FontWeight', 'bold');
ylabel('Power Demand (kW)', 'FontSize', 12, 'FontWeight', 'bold');
sgtitle('    Algoritma Oncesi');





function iGecenSure = gecenSureyiHesapla(iEnerjiBilgisi, iIstasyonBilgisi, iSocMax, iSocBilgisi)
    iGecenSure = floor((((iEnerjiBilgisi / iIstasyonBilgisi)/100)*(iSocMax - iSocBilgisi)) * 60);
end

function iGerekenEnerji = gerekenEnerjiyiHesapla(iEnerjiBilgisi,iSocMax, iSocBilgisi)
        iGerekenEnerji = ((iSocMax - iSocBilgisi) / 100) * iEnerjiBilgisi;
end

function bGerekenEnerjiSabitMi = hesaplananEnerjiSabitMi(iEnerjiBilgisi, iIstasyonBilgisi, iSocMax, iSocBilgisi)
         iOptimizasyonsuzEnerji = gerekenEnerjiyiHesapla(iEnerjiBilgisi, iSocMax, iSocBilgisi); 
         iOptimizasyonluEnerji = gecenSureyiHesapla(iEnerjiBilgisi, iIstasyonBilgisi, iSocMax, iSocBilgisi) * iIstasyonBilgisi;
         if iOptimizasyonsuzEnerji == iOptimizasyonluEnerji
               bGerekenEnerjiSabitMi = true;
         else
               bGerekenEnerjiSabitMi = false;
         end
end

