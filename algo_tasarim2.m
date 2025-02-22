% Tanimlamalar ve setlemeler
sDosyaAdi = 'household_power_consumption.txt';  
cAyrac = ';';

iSocMax = 80;

ist1GirisDkBaslangic = randi([500 600]);
ist2GirisDkBaslangic = randi([500 600]);
ist3GirisDkBaslangic = 750;

iIlkIstasyonAracGirisZamani = [112 447 702 755 823 868 1080 1150 1200 1249];
iIkinciIstasyonAracGirisZamani = [542 717 855 1131 1171 1216 1389];
iUcuncuIstasyonAracGirisZamani = [304 632 699 785 954 1276 1342 1408];

ist1AracSayisi = 10;
ist2AracSayisi = 7;
ist3AracSayisi = 8;

sure = 1440; % 1 gunun dakika cinsinden degeri

active_power_data = zeros(1, sure);
reactive_power_data = zeros(1, sure);
total_power_data = zeros(1, sure);

isebekeKatsayisi = 125;

istasyon_guc_bilgisi1 = 180; % kW
istasyon_guc_bilgisi2 = 200; % kW
istasyon_guc_bilgisi3 = 120; % kW

iAtlanacakGun = 0; % okunulan dosyada okunacak gunun kac gunluk atlanilacaginin katsayisi

if iAtlanacakGun ~= 0
    iBaslangicGun = length(active_power_data) * iAtlanacakGun;
    iBitisGun = length(active_power_data) * (iAtlanacakGun + 1) - 1;

else
    iBaslangicGun = 1;
    iBitisGun = length(active_power_data);
end

iIkiAracArasiZamanFarkiAraligi = [45 60];
iGelenAracEnerjiDegerAraligi = [60 100];
iGelenAracSocDegerAraligi = [0 20];



% Dosyanin okundugu yer
data = readtable(sDosyaAdi, 'Delimiter', cAyrac);
min = 1:1:sure; % 1 günlük zaman vektörü (saniye cinsinden)

% dosyadan okunan verinin eklendigi yer
j = 1;
for i = iBaslangicGun: iBitisGun
    active_power_data(j) = data{i,3} * isebekeKatsayisi;
    reactive_power_data(j) = data{i,4} *isebekeKatsayisi;
    total_power_data(j) = sqrt(power(active_power_data(i), 2) + power(reactive_power_data(i), 2));
    j = j + 1;
end

% sarj istasyonlarinan gelen bilgilerle struct doldurma islemi
sarjIstasyonlari = struct();
araclar = struct();
for i = 1: ist1AracSayisi
    araclar(i).iSocBilgisi = randi(iGelenAracSocDegerAraligi);
    araclar(i).iEnerjiBilgisi = randi(iGelenAracEnerjiDegerAraligi);
    %araclar(i).iDakikaGiris = ist1GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi); 
    araclar(i).iDakikaGiris = iIlkIstasyonAracGirisZamani(i);
    araclar(i).iDakikaGecenSure = gecenSureyiHesapla(araclar(i).iEnerjiBilgisi, istasyon_guc_bilgisi1, iSocMax, araclar(i).iSocBilgisi);
    araclar(i).iGuc = istasyon_guc_bilgisi1;
    %ist1GirisDkBaslangic = mod(ist1GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi),sure);
end
sarjIstasyonlari(1).aracBilgileri = araclar;
araclar = struct();
for i = 1: ist2AracSayisi
    araclar(i).iSocBilgisi = randi(iGelenAracSocDegerAraligi);
    araclar(i).iEnerjiBilgisi = randi(iGelenAracEnerjiDegerAraligi);
    %araclar(i).iDakikaGiris = ist2GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi);
    araclar(i).iDakikaGiris = iIkinciIstasyonAracGirisZamani(i);
    araclar(i).iDakikaGecenSure = gecenSureyiHesapla(araclar(i).iEnerjiBilgisi, istasyon_guc_bilgisi2, iSocMax, araclar(i).iSocBilgisi);
    araclar(i).iGuc = istasyon_guc_bilgisi2;
    %ist2GirisDkBaslangic = mod(ist2GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi), sure);
end

sarjIstasyonlari(2).aracBilgileri = araclar;
araclar = struct();
for i = 1: ist3AracSayisi
    araclar(i).iSocBilgisi = randi(iGelenAracSocDegerAraligi);
    araclar(i).iEnerjiBilgisi = randi(iGelenAracEnerjiDegerAraligi);
    %araclar(i).iDakikaGiris = ist3GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi);
    araclar(i).iDakikaGiris = iUcuncuIstasyonAracGirisZamani(i);
    araclar(i).iDakikaGecenSure = gecenSureyiHesapla(araclar(i).iEnerjiBilgisi, istasyon_guc_bilgisi3, iSocMax, araclar(i).iSocBilgisi);
    araclar(i).iGuc = istasyon_guc_bilgisi3;
    %ist3GirisDkBaslangic = mod(ist3GirisDkBaslangic + randi(iIkiAracArasiZamanFarkiAraligi), sure); 
end
sarjIstasyonlari(3).aracBilgileri = araclar;

active_power_data_temp = active_power_data;
[~, sarjIstasyonlariSize] = size(sarjIstasyonlari);
m = 1;
for i = 1: length(active_power_data)
    for j = 1 : sarjIstasyonlariSize
       [~, aracSize] = size(sarjIstasyonlari(j).aracBilgileri);
       for k = 1 : aracSize
            if( sarjIstasyonlari(j).aracBilgileri(k).iDakikaGiris == i)
                active_power_data_temp(i) = active_power_data_temp(i) + sarjIstasyonlari(j).aracBilgileri(k).iGuc;
                for m=1:sarjIstasyonlari(j).aracBilgileri(k).iDakikaGecenSure
                    active_power_data_temp(i + m) = active_power_data_temp(i + m) + sarjIstasyonlari(j).aracBilgileri(k).iGuc;
                end
            end         
        end
    end
end



% doldurulan structlar icin araclarin sarj bilgisi cizdirme islemi
arac_graf = zeros(1, sure);
istasyonlar(1, sure) = struct('istasyon1',0,  'istasyon2', 0, 'istasyon3', 0);

[~, sarjIstasyonlariSize] = size(sarjIstasyonlari);
for i = 1:length(active_power_data)
    for j = 1 : sarjIstasyonlariSize
       [~, aracSize] = size(sarjIstasyonlari(j).aracBilgileri);
       for k = 1 : aracSize
         if( sarjIstasyonlari(j).aracBilgileri(k).iDakikaGiris == i)
             arac_graf(i) = arac_graf(i) + sarjIstasyonlari(j).aracBilgileri(k).iGuc;
             if j == 1
                istasyonlar(i).istasyon1 = 1;
             end
             if j == 2
                istasyonlar(i).istasyon2 = 1;
             end
             if j == 3
                istasyonlar(i).istasyon3 = 1;
             end
            for m=1:sarjIstasyonlari(j).aracBilgileri(k).iDakikaGecenSure
                arac_graf(i + m) = arac_graf(i + m) + sarjIstasyonlari(j).aracBilgileri(k).iGuc;
                 if j == 1
                    istasyonlar(i + m).istasyon1 = 1;
                 end
                 if j == 2
                    istasyonlar(i + m).istasyon2 = 1;
                 end
                 if j == 3
                    istasyonlar(i + m).istasyon3 = 1;
                 end
            end     
         end       
       end
    end    
end

total_power_data_son = sqrt(active_power_data_temp.^2 + reactive_power_data.^2);
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




active_power_data_temp2 = active_power_data_temp;
arac_graf_temp = arac_graf;
istasyonlarSureyeGoreDolmusHali(3, 1440) = struct('iSoc',0,  'iEnerji', 0, 'iGiris', 0,'iGecenSure', 0);
k = 0;
[~, aracSize1] = size(sarjIstasyonlari(1).aracBilgileri);
[~, aracSize2] = size(sarjIstasyonlari(2).aracBilgileri);
[~, aracSize3] = size(sarjIstasyonlari(3).aracBilgileri);
for i = 1: length(active_power_data)
    if(istasyonlar(i).istasyon1 == 1)
        for j = 1: aracSize1
            if i == sarjIstasyonlari(1).aracBilgileri(j).iDakikaGiris
                  for m=0:sarjIstasyonlari(1).aracBilgileri(j).iDakikaGecenSure
                      istasyonlarSureyeGoreDolmusHali(1,i + m).iEnerji =  sarjIstasyonlari(1).aracBilgileri(j).iEnerjiBilgisi;
                      istasyonlarSureyeGoreDolmusHali(1,i + m).iSoc =  sarjIstasyonlari(1).aracBilgileri(j).iSocBilgisi;
                      istasyonlarSureyeGoreDolmusHali(1,i + m).iGecenSure =  sarjIstasyonlari(1).aracBilgileri(j).iDakikaGecenSure;
                      istasyonlarSureyeGoreDolmusHali(1,i + m).iGiris =  sarjIstasyonlari(1).aracBilgileri(j).iDakikaGiris;
                  end
            end        
        end
    end
    if(istasyonlar(i).istasyon2 == 1)
        for j = 1: aracSize2
            if i == sarjIstasyonlari(2).aracBilgileri(j).iDakikaGiris
                  for m=0:sarjIstasyonlari(2).aracBilgileri(j).iDakikaGecenSure
                      istasyonlarSureyeGoreDolmusHali(2,i + m).iEnerji =  sarjIstasyonlari(2).aracBilgileri(j).iEnerjiBilgisi;
                      istasyonlarSureyeGoreDolmusHali(2,i + m).iSoc =  sarjIstasyonlari(2).aracBilgileri(j).iSocBilgisi;
                      istasyonlarSureyeGoreDolmusHali(2,i + m).iGecenSure =  sarjIstasyonlari(2).aracBilgileri(j).iDakikaGecenSure;
                      istasyonlarSureyeGoreDolmusHali(2,i + m).iGiris =  sarjIstasyonlari(2).aracBilgileri(j).iDakikaGiris;
                  end
            end        
        end
    end
    if(istasyonlar(i).istasyon3 == 1)
        for j = 1: aracSize3
            if i == sarjIstasyonlari(3).aracBilgileri(j).iDakikaGiris
                  for m=0:sarjIstasyonlari(3).aracBilgileri(j).iDakikaGecenSure
                      istasyonlarSureyeGoreDolmusHali(3,i + m).iEnerji =  sarjIstasyonlari(3).aracBilgileri(j).iEnerjiBilgisi;
                      istasyonlarSureyeGoreDolmusHali(3,i + m).iSoc =  sarjIstasyonlari(3).aracBilgileri(j).iSocBilgisi;
                      istasyonlarSureyeGoreDolmusHali(3,i + m).iGecenSure =  sarjIstasyonlari(3).aracBilgileri(j).iDakikaGecenSure;
                      istasyonlarSureyeGoreDolmusHali(3,i + m).iGiris =  sarjIstasyonlari(3).aracBilgileri(j).iDakikaGiris;
                  end
            end        
        end
    end
   
end



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

