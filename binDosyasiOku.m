% İstasyonlara giren araçlar
mIstasyonlaraGirenAraclar = {
    [112, 447, 702, 755, 823, 868, 900, 960, 1040, 1095, 1150, 1200, 1249];
    [542, 717, 855, 900, 975, 1150, 1200, 1300, 1389];
    [304, 632, 699, 900, 954, 1150, 1276, 1342, 1408];
};

% Dosyayı aç
fid = fopen('Algo.bin', 'rb');

% 1440 adet yapı tanımla

istasyonlar(length(mIstasyonlaraGirenAraclar), 1440) = struct('Sure', 0, 'HamGuc', 0, 'IstasyonNo', 0, 'AracNo',0, 'Soc', 0, 'Enerji', 0, 'GirisDk', 0, 'IstasyonGuc', 0, 'SarjSuresi',0,'BinmisYuk', 0);
for m = 1:1440
    Dakika = fread(fid, 1, 'int32');
    sebekeHamGuc = fread(fid, 1, 'single');
   
    for i = 1:length(mIstasyonlaraGirenAraclar)
        nArac = length(mIstasyonlaraGirenAraclar{i});
        for j = 1:nArac
            istasyonlar(i,m).Sure = Dakika;
            istasyonlar(i,m).HamGuc= sebekeHamGuc;
            
                istNo = fread(fid, 1, 'int32');
                AracNo = fread(fid, 1, 'int32');
                Soc = fread(fid, 1, 'single');
                Enerji = fread(fid, 1, 'single');
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
    
    binmisYuk = fread(fid, 1, 'single');
    istasyonlar(1,m).BinmisYuk = binmisYuk;
    istasyonlar(2,m).BinmisYuk = binmisYuk;
    istasyonlar(3,m).BinmisYuk = binmisYuk;
end

fclose(fid);
