% İstasyonlara giren araçlar
mIstasyonlaraGirenAraclar = {
    [112, 447, 702, 755, 823, 868, 900, 960, 1040, 1095, 1150, 1200, 1249];
    [542, 717, 855, 900, 975, 1150, 1200, 1300, 1389];
    [304, 632, 699, 900, 954, 1150, 1276, 1342, 1408];
};

% Dosyayı aç
fid = fopen('Algo.bin', 'rb');

% 1440 adet yapı tanımla

istasyonlar(length(mIstasyonlaraGirenAraclar), 1440) = struct('Sure', 0, 'HamGuc', 0, 'IstasyonNo', 0, 'AracNo',0, 'Soc', 0, 'Enerji', 0, 'GirisDk', 0, 'IstasyonGuc', 0, 'IstasyonTotalGucAlgoSonra', 0, 'IstasyonHamGuc', 0, 'SarjSuresi',0,'BinmisYuk', 0, 'AlgoOncesiBinmisYuk',0);
for m = 1:1440
    Dakika = fread(fid, 1, 'int32');
    sebekeHamGuc = fread(fid, 1, 'float32');
    istasyonHamGucTotal = fread(fid, 1, 'int32');
      
    for i = 1:length(mIstasyonlaraGirenAraclar)
        nArac = length(mIstasyonlaraGirenAraclar{i});
        for j = 1:nArac
            istasyonlar(i,m).Sure = Dakika;
            istasyonlar(i,m).HamGuc= sebekeHamGuc;
            istasyonlar(i,m).IstasyonHamGuc = istasyonHamGucTotal;            
            istNo = fread(fid, 1, 'int32');
            AracNo = fread(fid, 1, 'int32');
            Soc = fread(fid, 1, 'float32');
            Enerji = fread(fid, 1, 'float32');
            GirisDk = fread(fid, 1, 'int32');
            IstasyonGuc= fread(fid, 1, 'int32');
            SarjSuresi = fread(fid, 1, 'int32');
            if istNo > 0
                istasyonlar(i,m).IstasyonNo = istNo;
                istasyonlar(i,m).AracNo = AracNo;
                istasyonlar(i,m).Soc = Soc;
                istasyonlar(i,m).Enerji = Enerji;
                istasyonlar(i,m).GirisDk = GirisDk;
                istasyonlar(i,m).IstasyonGuc = IstasyonGuc;
                istasyonlar(i,m).SarjSuresi = SarjSuresi;
            end
            
        end
    end 
    
    binmisYuk = fread(fid, 1, 'float32');
    AlgoOncesiBinmisYuk = fread(fid, 1, 'float32');
    AlgoSonrasiIstasyonlarGuc = fread(fid, 1, 'int32');
    istasyonlar(1,m).IstasyonTotalGucAlgoSonra = AlgoSonrasiIstasyonlarGuc;
    istasyonlar(2,m).IstasyonTotalGucAlgoSonra = AlgoSonrasiIstasyonlarGuc;
    istasyonlar(3,m).IstasyonTotalGucAlgoSonra = AlgoSonrasiIstasyonlarGuc;
    istasyonlar(1,m).BinmisYuk = binmisYuk;
    istasyonlar(2,m).BinmisYuk = binmisYuk;
    istasyonlar(3,m).BinmisYuk = binmisYuk;
    istasyonlar(1,m).AlgoOncesiBinmisYuk = AlgoOncesiBinmisYuk;
    istasyonlar(2,m).AlgoOncesiBinmisYuk = AlgoOncesiBinmisYuk;
    istasyonlar(3,m).AlgoOncesiBinmisYuk = AlgoOncesiBinmisYuk;
end

fclose(fid);

binmisYukVektoru = zeros(1, 1440);  % 1440 dakikalık boş vektör oluştur
AlgoOncesiBinmisYukVektoru = zeros(1, 1440);  % 1440 dakikalık boş vektör oluştur
IstasyonHamGucVektoru = zeros(1,1440);
IstasyonGucVektoru = zeros(1,1440);
HamGuc = 0;
Guc = 0;
for m = 1:1440
    binmisYukVektoru(m) = istasyonlar(1, m).BinmisYuk;  % herhangi bir istasyondan alabilirsin, hepsi aynı
    AlgoOncesiBinmisYukVektoru(m) = istasyonlar(1, m).AlgoOncesiBinmisYuk;
    IstasyonHamGucVektoru(m) = istasyonlar(1, m).IstasyonHamGuc;
    IstasyonGucVektoru(m) = istasyonlar(1, m).IstasyonTotalGucAlgoSonra;
end


% Grafiği çiz
figure;
plot(1:1440, binmisYukVektoru, 'b', 'LineWidth', 1.5);
xlabel('Dakika');
ylabel('Binmiş Yük');
title('Binmiş Yük - Dakika Grafiği');
grid on;
hold on;
plot(1:1440, AlgoOncesiBinmisYukVektoru, 'r', 'LineWidth', 1.5)

figure;
plot(1:1440, IstasyonHamGucVektoru, 'b', 'LineWidth', 1.5);
xlabel('Dakika');
ylabel('İStasyon Yükü');
hold on;
plot(1:1440, IstasyonGucVektoru, 'r', 'LineWidth', 1.5);