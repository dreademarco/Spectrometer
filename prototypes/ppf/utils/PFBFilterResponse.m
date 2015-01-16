fs = 1024; %sample rate (MHz )
dec = 32; %decimation factor
taps = 2^8; %number of taps

%time array
t = 0:(1/fs):((1/fs)*(taps-1));

%Polyphase prototype filter
s=fir1(taps-1,1/dec);

%plot the prototype filter
%figure(3), plot(t,s);

%zero pad prototpye filter up by factor of 100 and calc spectrum
%S = fftshift(fft((s)));
S = fftshift(fft((s),100*taps));

%frequency array of spectrum
%f = ((-floor(taps/2):(ceil(taps/2)-1))/(taps))/(t(2)-t(1));
f = ((-floor(100*taps/2):(ceil(100*taps/2)-1))/(100*taps))/(t(2)-t(1));

%plot the spectrum (dB)
figure(1); plot(f,20*log10(abs(S)));
% axis([-16 16 -0 1.1])

%figure(2), plot(f,angle(S.*exp(1i*2*pi*f*(127.5/1024)))*180/pi);
figure(2), plot(f,angle(S.*exp(1i*2*pi*f*(127.5/1024))));
%axis([-16 16 -189 180])

% write spectrum to file
% dlmwrite('PFB32response.txt',[f' real(S)' imag(S)'],'newline','unix');
Sps = S.*exp(1i*2*pi*f*(127.5/1024));
Sps(angle(Sps) ~= 0) = abs(Sps(angle(Sps) ~= 0));

St = zeros(16,length(Sps));
for n=1:16
	St(n,:) = circshift(Sps',(n-1)*floor(dec/f(2)-f(1)))';
end

St = St/max(max(St)); %normalize
figure(3),
plot(f/512,St);
axis([0 1 0 1.01])
xlabel('Frequency (half-cycles/second)')
ylabel('Normalized Gain');
title('Matlab Prototype PFB');
%grid

figure(4),
plot(f,angle(St),'linewidth',5);
axis([0 512 -10 10]);
xlabel('Frequency (Mhz)');
ylabel('Phase (\theta)');
%grid