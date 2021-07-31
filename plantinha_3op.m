%%%%%%%%% 3º opção %%%%%%%%%%

fs=500; %%frequencia que o sinal ecg foi amostrado 
Num=1;
T=1/fs;

x = load('rec_4m.mat'); %%pessoa 1, sinal 4
x1 = x.val(1,:);

L=length(x1) %%tamanho do sinal de ecg
t=(0:L-1)*T; %%para colocar o vetor do sinal no dominio do tempo continuo

%quanto maior a ordem do filtro, menor é a largura do lobulo principal
%central(resposta no tempo do filtro FIR) e menor é a banda de transicao (https://www2.slideshare.net/amrelsayedm/dsp2018foehu-lec-06-fir-filter-design)

%O motivo pelo qual quanto maior a ordem do filtro, mais ele se aproxima de
%um filtro ideal, é o fato de que em um filtro ideal no dominio do tempo
%discreto, o eixo das amostras deve ser de -infinito a +infinito

X = fft(x1); %transformada de fourier de x -> a funcao fft pode ser usada para sinal no tempo continuo e discreto
L = length(X);
P2 = abs(X/L); %o sinal é espelhado -> 0-L/2 = L/2-L
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);
f = fs*(0:(L/2))/L;
subplot(5,1,2)
plot(f,P1) 
ylim([0 15])
title('Frequency Response of the Signal')
xlabel('Frequency [Hz]')
ylabel('Magnitude')

subplot(5,1,1)
plot(t,x1,'m')
ylim([-100 200])
title('Signal')
xlabel('Time [s]')
ylabel('Amplitude [mV]')

%filtro passa-alta
Den = fir1(80,0.5/(fs/2),'high',blackmanharris(81)); 
y = filter(Den,Num,x1);

z2 = fft(y); %transformada de fourier de x -> a funcao fft pode ser usada para sinal no tempo continuo e discreto
L = length(z2);
P222 = abs(z2/L); %o sinal é espelhado -> 0-L/2 = L/2-L
P111 = P222(1:L/2+1);
P111(2:end-1) = 2*P111(2:end-1);
f = fs*(0:(L/2))/L;
 
subplot(5,1,3)
plot(f,P111) 
ylim([0 15])
title('Frequency Response of the High-Pass FIR Filtered Signal')
xlabel('Frequency [Hz]')
ylabel('Magnitude')

%filtro passa-baixa
Den1 = fir1(80,40/(fs/2),'low',chebwin(81,60)); 
g = filter(Den1,Num,y);

delay1 = mean(grpdelay(Den1)+grpdelay(Den))
tt1 = t(1:end-delay1);
sn1 = g(1:end-delay1);
sg = g;
sg(1:delay1) = [];

z3 = fft(sg); %transformada de fourier de x -> a funcao fft pode ser usada para sinal no tempo continuo e discreto
L = length(z3);
P2222 = abs(z3/L); %o sinal é espelhado -> 0-L/2 = L/2-L
P1111 = P2222(1:L/2+1);
P1111(2:end-1) = 2*P1111(2:end-1);
f = fs*(0:(L/2))/L;

subplot(5,1,4)
plot(f,P1111) 
ylim([0 15])
title('Frequency Response of the Low-Pass FIR Filtered Signal')
xlabel('Frequency [Hz]')
ylabel('Magnitude')

subplot(5,1,5)
plot(tt1,sg,'m')
ylim([-100 200])
title('Final Signal')
xlabel('Time [s]')
ylabel('Amplitude [mV]')

%%subtracao do sinal original menos o sinal filtrado da base de dados para obter SNR

x2=x.val(2,:);

noise = x1-x2;

SNR = snr(x2,noise);
%snrrr = 10*log10((sum(x2.^2))/(sum(noise.^2)))


    if(length(noise)>length(sg))
        sg((length(sg)):length(noise))=0;
    end 

SNRf = snr(sg,noise);

%%correlação entre o sinal filtrado e o sinal filtrado da base de dados

c = sum((abs(x2 - sg)).^2);
cn = c/(sum((abs(x2)).^2));
