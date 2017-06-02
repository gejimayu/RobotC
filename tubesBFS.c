#pragma config(StandardModel, "EV3_REMBOT")
#pragma DebuggerWindows("EV3LCDScreen") // (documented for NXT but not for EV3).

//----------------------ADT QUEUE--------------------------------
/* Definisi Queue kosong: HEAD=Nil; TAIL=Nil. */
/* Catatan implementasi: T[0] tidak pernah dipakai */

/* ********* AKSES (Selektor) ********* */
/* Jika Q adalah Queue, maka akses elemen : */
#define Head(Q) (Q).HEAD
#define Tail(Q) (Q).TAIL
#define InfoHead(Q) (Q).Parent[(Q).HEAD]
#define InfoTail(Q) (Q).Parent[(Q).TAIL]
#define Nil 0
#define red 1
#define green 2
#define blue 3
#define yellow 4
#define black 5
#define white 6
#define tolerance 20

typedef struct {
	int Pi[20];
	int Pa[20];
	int indeks[20];
	int nPath;
} path;

typedef struct {
	path T;
	int Child[4];
	int nChild;
} tipeX;

typedef struct { tipeX Parent[20];
                 int HEAD;
                 int TAIL;
               } Queue;

//global variable
path Jalur; //jalur yang dilalui robot
Queue Q; //Queue BFS

void addPath(tipeX *X){
	int i;
	(*X).T.nPath = Jalur.nPath;
	for(i = 1; i <= Jalur.nPath; i++){
		(*X).T.Pi[i] = Jalur.Pi[i];
		(*X).T.Pa[i] = Jalur.Pa[i];
		(*X).T.indeks[i] = Jalur.indeks[i];
	}
}

void createEmpty(){
	Head(Q) = 0;
	Tail(Q) = 0;
}

int isEmpty() {
/* Mengirim true jika Q kosong: lihat definisi di atas */
	return(Head(Q)==Nil && Tail(Q)==Nil);
}

void add (tipeX X)
{
	int i;
	if (isEmpty())
	{
		Head(Q) = 1;
	}
	Tail(Q)++;
	//copy path
	InfoTail(Q).T.nPath = X.T.nPath;
	for(i = 1; i <= X.T.nPath; i++){
		InfoTail(Q).T.Pi[i] = X.T.Pi[i];
		InfoTail(Q).T.Pa[i] = X.T.Pa[i];
		InfoTail(Q).T.indeks[i] = X.T.indeks[i];
	}
	//copy child
	InfoTail(Q).nChild = X.nChild;
	for(i = 1; i <= X.nChild; i++){
		InfoTail(Q).Child[i] = X.Child[i];
	}
}

void del (tipeX *X)
{
	int i;
	//copy path
	(*X).T.nPath = InfoHead(Q).T.nPath;
	for(i = 1; i <= InfoHead(Q).T.nPath; i++){
		(*X).T.Pi[i] = InfoHead(Q).T.Pi[i];
		(*X).T.Pa[i] = InfoHead(Q).T.Pa[i];
		(*X).T.indeks[i] = InfoHead(Q).T.indeks[i];
	}
	//copy child
	(*X).nChild = InfoHead(Q).nChild;
	for(i = 1; i <= InfoHead(Q).nChild; i++){
		(*X).Child[i] = InfoHead(Q).Child[i];
	}
	//urus head dan tail
	if (Head(Q) == Tail(Q))
	{
		Head(Q) = Nil;
		Tail(Q) = Nil;
	}
	else
	{
		Head(Q)++;
	}
}

//---------------------------------------------------------------

void displayJalur(){
	int j;
	for(j = 2; j <= Jalur.nPath; j++)
		displayTextLine(j+1,"%d",Jalur.indeks[j]);
	for(j = Jalur.nPath+1; j <= 10; j++)
		displayTextLine(j+1," ");
}
int isLight(){
	return (getColorReflected(colorSensor) < 20);
}

void belokKiri(){
	motor[rightMotor] = 15;
	motor[leftMotor] = 55;
}

void belokKanan(){
	motor[rightMotor] = 55;
	motor[leftMotor] = 15;
}

void moveForward(int distance){
	moveMotorTarget(rightMotor,distance,50);
	moveMotorTarget(leftMotor,distance,50);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) )
		sleep(1);
}

void balikArah(){
	setMotorSpeed(leftMotor, -100);
	setMotorSpeed(rightMotor, 100);
	sleep(500);
}

void getDegree(int *s){
	*s = getGyroHeading(gyroSensor);
}

void muter(int s){
	int sudut;
	getDegree(&sudut);
	while (sudut != s){
		motor[leftMotor] = 20;
		motor[rightMotor] = -20;
		getDegree(&sudut);
	}
}

int detectColor(){
	int r,g,b;
	getColorRGB(colorSensor,r,g,b);
	if ( (b > g + tolerance) && (b > r + tolerance) )
		return blue;
	else
	if ( (g > b + tolerance) && (g > r + tolerance) )
		return green;
	else
	if ( (r > g + tolerance) && (r > b + tolerance ) )
		return red;
	else
	if ((r > 100-tolerance) && (r < 100+tolerance) &&  (g > 100-tolerance) && (g < 100+tolerance) && (b > 100-tolerance) && (b < 100+tolerance) )
		return white;
	else
	if ( (r > 100 - tolerance) && (g > 100 - tolerance) && (b < 0 + tolerance) )
		return yellow;
	else
		return black;
}

void initialMove(){
	moveForward(900);
}

void lewatinIjo(){
	while (detectColor() == green) { //maju sampe kotak green terlewati
		moveForward(40);
	}
}

int cekKanan(int s){
	int found,fck;
	found = 0;
	while (found == 0){
		if (detectColor() == black){
			turnRight(100,degrees,50);
			getDegree(&fck);
			found = 1;
		}
		else {
			motor[leftMotor] = 20;
			motor[rightMotor] = -20;
		}
	}
	muter(s);
	return fck;
}

int cekKiri(int s){
	int found,fck;
	found = 0;
	while (found == 0){
		if (detectColor() == black){
			getDegree(&fck);
			found = 1;
		}
		else {
			motor[leftMotor] = -20;
			motor[rightMotor] = 20;
		}
	}
	muter(s);
	return fck;
}

int findNode(){
	int tileColor,s;
	displayText(line1,"Lagi cari node");
	while (true) {
		if (isLight()){
			belokKiri();
		}
		else {
			belokKanan();
		}
		getDegree(&s);
		tileColor = detectColor();
		if ((tileColor != black) && (tileColor != white)) return tileColor;
	}
}

void pergi(){
	displayText(line1,"Kunjungi node berikutnya");
	int i,mulai;
	i = 1;
	if (Jalur.nPath == 0){
		mulai = 1;
	}
	else{
		while ( (i <= Jalur.nPath) && (InfoHead(Q).T.Pi[i] == Jalur.Pi[i]) && (InfoHead(Q).T.Pa[i] == Jalur.Pa[i]) )
			i++;
		mulai = i;
	}
	for(i = mulai; i <= InfoHead(Q).T.nPath; i++){ //jalanin sisa
		muter(InfoHead(Q).T.Pi[i]);
		Jalur.nPath++; //update jalur
		Jalur.Pi[Jalur.nPath] = InfoHead(Q).T.Pi[i]; //tambahin jalur arah awal
		Jalur.Pa[Jalur.nPath] = InfoHead(Q).T.Pa[i]; //update jalur arah akhir
		Jalur.indeks[Jalur.nPath] = InfoHead(Q).T.indeks[i];
		displayTextLine(5,"%d %d",InfoHead(Q).T.indeks[i], i);
		displayJalur();
		lewatinIjo();
		findNode();
		lewatinIjo();
	}
}

void pulangKeCabang(tipeX pop){
	displayText(line1,"Kembali ke node sebelumnya");
	int i,sampe;
	if (isEmpty()){
		sampe = pop.T.nPath;
	}
	else{
		i = 1;
		while ((i <= pop.T.nPath) && (pop.T.Pi[i] == InfoHead(Q).T.Pi[i]) && (pop.T.Pa[i] == InfoHead(Q).T.Pa[i]) )
			i++;
		sampe = i;
	}
	for(i = pop.T.nPath; i >= sampe; i--){
		muter((pop.T.Pa[i] + 180) % 360);
		lewatinIjo();
		findNode();
		Jalur.nPath--;
		displayJalur();
	}
}

void pulang(){
	displayText(line1,"Kembali ke pintu masuk!");
	int i;
	for (i = Jalur.nPath; i >= 1; i--) {
		muter( (Jalur.Pa[i] + 180) % 360);
		lewatinIjo();
		findNode();
		lewatinIjo();
	}
}

void pushChild(tipeX *push, int cabang){
	(*push).nChild++;
	(*push).Child[(*push).nChild] = cabang;  //masukan anak cabang
}

void branching(){
	int s,cek,cabang;
	tipeX push;
	push.nChild = 0; //cabang anak
	//masukan path ke push
	addPath(&push);
	displayText(line1,"Sedang mengecek cabang node");
	lewatinIjo();
	moveForward(80); //cek depan
	if (detectColor() == black){ //found branch
		getDegree(&cabang);
		if (cabang < 0) cabang += 360;
		pushChild(&push,cabang);
		 //cek kanan
		turnRight(200,degrees,50);
		lewatinIjo();
		moveForward(50);
		if (detectColor() == black){ //found another branch
			getDegree(&cabang);
			if (cabang < 0) cabang += 360;
			pushChild(&push,cabang);
		}
		moveForward(-50);

		 //cek kiri
		turnLeft(200,degrees,50);
		moveForward(-93); //kembali ke posisi awal
		turnLeft(220,degrees,50);
		moveForward(50);
		lewatinIjo();
		if (detectColor() == black){ // found another branch
			getDegree(&cabang);
			if (cabang < 0) cabang += 360;
			pushChild(&push,cabang);
		}
		moveForward(-50);
		turnRight(240,degrees,50);  //balik ke posisi awal
	}
	else {
		getDegree(&s);
		cek = cekKanan(s);
		if (cek < 0) cek += 360;
		pushChild(&push,cek);
		getDegree(&s);
		cek = cekKiri(s);
		if (cek < 0) cek += 360;
		pushChild(&push,cek);
	}
	add(push); //tambah ke queue
}

void BFS(){
	int tileColor,i,s,arahpulang,found;
	found = 0; //api blom ditemukan
	tipeX pop;
	tileColor = findNode();
	if (tileColor == green) {  //kasus khusus, persimpangan awal
		getDegree(&s); //ambil sudut awal
		Jalur.Pa[Jalur.nPath] = s; //arah akhir jalur robot pada branch
		branching();
	}
	while (!isEmpty() && !found){
		pergi();
		del(&pop);
		for(i = 1; i <= pop.nChild && !found ; i++){
			displayText(line1,"Sedang menelusuri cabang");
			muter(pop.Child[i]); //arahin robot
			Jalur.nPath++; //update jalur
			Jalur.indeks[Jalur.nPath] = i;
			Jalur.Pi[Jalur.nPath] = pop.Child[i];
			displayJalur();
			lewatinIjo();
			tileColor = findNode();
			if (tileColor == green) {
				getDegree(&s);
				Jalur.Pa[Jalur.nPath] = s; //tambahin jalur arah akhir
				branching();
				displayText(line1,"Kembali ke simpangan awal");
				Jalur.nPath--; //hapus jalur terakhir
				arahpulang = (pop.Child[i] + 180) % 360; //balikin arah
				muter(arahpulang); //cari arah pulang
				lewatinIjo();
				findNode();
				lewatinIjo();
			}
			else
			if (tileColor == red) {
				displayText(line1,"Jalan buntu, kembali");
				Jalur.nPath--;
				balikArah();
				findNode();
				lewatinIjo();
			}
			else
			if (tileColor == yellow) {
				displayText(line1,"Api ditemukan dan telah dipadamkan!");
				sleep(2000);
				found = 1; // api telah ditemukan
				//kembali
				Jalur.nPath--;
				balikArah();
				findNode();
				lewatinIjo();
				pulang();
			}
			displayJalur();
		}
		pulangKeCabang(pop); //pulang ke cabang terdekat
	}
	if (!found) pulang();
}

task main()
{
	int s;
	displayTextLine(2,"Path : ");
	createEmpty(); // bkin Queue
	Jalur.nPath = 1; //jalur awal pasti 1 langkah
	getDegree(&s);
	Jalur.Pi[Jalur.nPath] = s;
	initialMove();
	BFS();
}
