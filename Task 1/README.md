# Balatro-project
1.	What is the invariant structure in your program?  
Struktur invariant adalah RunSession, khususnya method run() yang mengatur urutan fase permainan secara tetap: generate input → hitung base score → hitung reward → update money → shop phase → lanjut ronde. Urutan ini tidak berubah meskipun komponen input, scoring, reward, atau shop diganti. Konstruktor RunSession juga hanya menerima pointer antarmuka, sehingga alur permainan tetap sama.

2.	Which parts are mutable?  
Bagian mutable adalah komponen yang bisa diganti tanpa mengubah RunSession, yaitu:
IInputGenerator, Contoh: (FixedInputGenerator dan RandomInputGenerator.)
IRewardRule, Contoh: (EqualRewardRule, BonusRewardRule, EvenRoundDoubleRewardRule.)
IScoreingRule, Contoh: (SimpleScoringRule.)
ShopSystem, Logika Shop.

3.	When you replaced the InputGenerator, why didn’t RunSession change? 
Karena RunSession hanya bergantung pada abstraksi IInputGenerator, bukan pada kelas konkret seperti FixedInputGenerator atau RandomInputGenerator. RunSession memanggil inputGen->generate(round) melalui pointer antarmuka. Selama kelas baru mengimplementasikan generate(), RunSession tidak perlu tahu detail implementasinya. Inilah prinsip polimorfisme dan dependency inversion.

4. What would happen if scoring logic was placed inside 
RunSession? 
Jika scoring ditulis langsung di RunSession, maka setiap perubahan aturan scoring mengharuskan RunSession diedit. Akibatnya:
RunSession, menjadi tidak stabil dan melanggar prinsip single responsibility.
Perubahan kecil pada scoring berisiko merusak alur permainan.
Sulit menguji atau mengganti strategi scoring secara terpisah.
Kode menjadi kurang fleksibel dan sulit dikembangkan.
