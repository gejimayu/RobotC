#pragma config(StandardModel, "EV3_REMBOT")
#pragma DebuggerWindows("EV3LCDScreen") // (documented for NXT but not for EV3).

//----------------------ADT QUEUE--------------------------------------

#define Nil 0
#define red 1
#define green 2
#define blue 3
#define yellow 4
#define black 5
#define white 6
#define tolerance 20

#define MaxEl 10
/* Nil adalah stack dengan elemen kosong . */
/* Karena indeks dalam bhs C dimulai 0 maka tabel dg indeks 0 tidak dipakai */

typedef int infotype;
typedef int address;   /* indeks tabel */

/* Contoh deklarasi variabel bertype stack dengan ciri TOP : */
/* Versi I : dengan menyimpan tabel dan alamat top secara eksplisit*/
typedef struct {
	infotype T[MaxEl+1]; /* tabel penyimpan elemen */
	address TOP;  /* alamat TOP: elemen puncak */
} Stack;
/* Definisi stack S kosong : S.TOP = Nil */
/* Elemen yang dipakai menyimpan nilai Stack T[1]..T[MaxEl] */
/* Jika S adalah Stack maka akses elemen : */
   /* S.T[(S.TOP)] untuk mengakses elemen TOP */
   /* S.TOP adalah alamat elemen TOP */

/* Definisi akses dengan Selektor : Set dan Get */
#define Top(S) (S).TOP
#define InfoTop(S) (S).T[(S).TOP]


bool FirstNode = true;


void CreateEmpty (Stack *S)
/* I.S. sembarang; */
/* F.S. Membuat sebuah stack S yang kosong berkapasitas MaxEl */
/* jadi indeksnya antara 1.. MaxEl+1 karena 0 tidak dipakai */
/* Ciri stack kosong : TOP bernilai Nil */
{
	Top(*S) = 0;
	InfoTop(*S) = Nil;
}
/* *********** Predikat Untuk test keadaan KOLEKSI ************ */
bool IsEmpty (Stack S)
/* Mengirim true jika Stack kosong: lihat definisi di atas */
{
	if(InfoTop(S) == Nil)
		return true;
	else
		return false;
}

bool IsFull (Stack S)
/* Mengirim true jika tabel penampung nilai elemen stack penuh */
{
	if(Top(S) == MaxEl)
		return true;
	else
		return false;
}

/* ************ Menambahkan sebuah elemen ke Stack ************ */
void Push (Stack * S, infotype X)
/* Menambahkan X sebagai elemen Stack S. */
/* I.S. S mungkin kosong, tabel penampung elemen stack TIDAK penuh */
/* F.S. X menjadi TOP yang baru,TOP bertambah 1 */
{
	Top(*S)++;
	InfoTop(*S) = X;
}
/* ************ Menghapus sebuah elemen Stack ************ */
void Pop (Stack * S, infotype* X)
/* Menghapus X dari Stack S. */
/* I.S. S  tidak mungkin kosong */
/* F.S. X adalah nilai elemen TOP yang lama, TOP berkurang 1 */
{
	*X = InfoTop(*S);
	Top(*S)--;
}
//---------------------------------------------------------------


int Path[100];
int nodeIndex=-1;

void DFS(Stack *S, Stack * NodeS);

int isLight(){
	return (getColorReflected(colorSensor) < 40);
}

void belokKiri(){
	motor[rightMotor] = 15;
	motor[leftMotor] = 55;
}

void belokKanan(){
	motor[rightMotor] = 55;
	motor[leftMotor] = 15;
}

void balikArah(){
	setMotorSpeed(leftMotor, -100);
	setMotorSpeed(rightMotor, 100);
	sleep(500);
}



void getDegree(int *s){
	*s = getGyroHeading(gyroSensor);
	//displayVariableValues(4,*s);
}

void muter(int s){
	int sudut;
	getDegree(&sudut);
	if(s<0)
		s+=360;
	while (sudut != s){
		motor[leftMotor] = 20;
		motor[rightMotor] = -20;
		getDegree(&sudut);
	}
}
void clearDisplay();
int detectColor(){
	int r,g,b;
	getColorRGB(colorSensor,r,g,b);
	//displayVariableValues(line1,r);
	//displayVariableValues(line2,g);
	//displayVariableValues(line3,b);
	if ( (b > g + tolerance) && (b > r + tolerance) )
		return blue;
	else
		if ( (g > b + tolerance) && (g > r + tolerance) )
		return green;
	else
		if ( (r > g + tolerance) && (r > b + tolerance ) )
		return red;
	else
		if( (r > 100-tolerance) && (g > 100 - tolerance) && (b < 0 + tolerance) )
			return yellow;
	else
		if ((r > 100-tolerance) && (r < 100+tolerance) &&  (g > 100-tolerance) && (g < 100+tolerance) && (b > 100-tolerance) && (b < 100+tolerance) )
		return white;
	else
		return black;
}

void moveForward(int distance){
	moveMotorTarget(rightMotor,distance,50);
	moveMotorTarget(leftMotor,distance,50);
	while ( getMotorMoving(leftMotor) || getMotorMoving(rightMotor) )
		sleep(1);
}

void initialMove(){
	moveForward(900);
}

int findNode(){
	int tileColor,s;
	while (true) {
		if (isLight() && (detectColor() != yellow)){
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


void backTrack(Stack *S){
	balikArah();
	int Dummy = findNode();
	do{
		moveForward(45);
	} while(detectColor() == green);
	wait(1,seconds);
	moveForward(50);
	int Deg;
	Pop(S,&Deg);
	//displayVariableValues(line1, Deg);
	muter(Deg);
	//displayText(line8,"BackTrackingComplete");
	//wait(1);
	nodeIndex--;
	displayText(line8, " ");
}

void displayPath();

void goHome(Stack *S){
	while(!IsEmpty(*S))
	{
		balikArah();
		//displayText(line4,"PULANG");
		int Dummy = findNode();
		int Deg;
		if(Dummy == green)
		{
			do {
				moveForward(40);
			} while (detectColor() == green);
			wait(1,seconds);
			moveForward(50);
		}
		Pop(S,&Deg);
		muter(Deg);
	}
	balikArah();
	int Dummy = findNode();
	do {
				moveForward(40);
	} while (detectColor() == blue);
	moveForward(50);
	clearDisplay();
	displayText(line1,"Jalan yang ditempuh untuk sampai api:");
	displayPath();
	displayText(line3, "Robot Telah Pulang");
	stopAllTasks();
}

void branching(Stack *S, Stack* NodeS){
	int initial,s;
	getDegree(&initial);
	//displayVariableValues(line5,initial);
	do {
		moveForward(40);
	} while (detectColor() == green);
	wait(1,seconds);
	moveForward(70); //cek depan
	getDegree(&initial);
	Push(NodeS,initial);
	if (detectColor() == black){ //found branch
		getDegree(&s);
	//	displayVariableValues(line6,s);
		wait(1,seconds);
		Push(S,s);
		Path[nodeIndex]++;
		DFS(S,NodeS);
//		wait(2,seconds);
		turnRight(220,degrees,50);
		if (detectColor() == black){ //found another branch
			getDegree(&s);
		//	displayVariableValues(line7,s);
			Push(S,s);
			Path[nodeIndex]++;
			DFS(S,NodeS);
		}
		//aswait(1,seconds);
		turnLeft(240+230,degrees,50);
		if (detectColor() == black){ // found another branch
			getDegree(&s);
			//displayVariableValues(line8,s);
			Push(S,s);
			Path[nodeIndex]++;
			DFS(S,NodeS);
		}

		//moveForward(-90);
	}
	else
	{
		//Kalo gak ada depannya dianggap pasti ada 2 cabang dikiri dan dikanan.
		//CekKiri
		int DEG;
		getDegree(&DEG);
		while(detectColor() != black)
		{
			motor[leftMotor] = -20;
			motor[rightMotor] = 20;
		//	displayText(line4,"MasukSINI");
		}
		getDegree(&s);
		Push(S,s);
		Path[nodeIndex]++;
		DFS(S,NodeS);
		muter(DEG);
		//turnRight(100,degrees,50);
		//DFS(S);
		//Cek Kanan
		while(detectColor() != black)
		{
			motor[leftMotor] = 20;
			motor[rightMotor] = -20;
		//	displayText(line4,"MasukSINI");
		}
		turnRight(100,degrees,50);
		getDegree(&s);
		Push(S,s);
		Path[nodeIndex]++;
		DFS(S,NodeS);
		//stopAllTasks();
	}
	Pop(NodeS,&initial);
	//displayText(line4,"Sedang Muter");
	muter(initial);
	Path[nodeIndex] = 0;
	//displayText(line4,"Selesai Muter");
	backTrack(S);
}

void clearDisplay()
{
	displayText(line1," ");
	displayText(line2," ");
	displayText(line3," ");
}

void displayPath()
{	//displayText(line1,"Path Taken Is:");
	displayTextLine(1,"%d-%d-%d-%d-%d-%d-%d-%d-%d-%d", Path[0],Path[1],Path[2],Path[3],Path[4],Path[5],Path[6],Path[7],Path[8],Path[9]);
}

void DFS(Stack *S, Stack* NodeS){
	nodeIndex++;
	displayText(line1,"Jalur yang diambil:");
	displayPath();
	int tileColor;
	tileColor = findNode();
	if (tileColor == green) {
		branching(S,NodeS);
		//backTrack(S);
	}
	else if (tileColor == red){
		backTrack(S);
	}
	else if (tileColor == yellow){
		//displayText(line1,"INI KUNING CUK");
		//goHome(NodeS);
		clearDisplay();
		displayText(line1, "Api telah dipadamkan!!!");
		displayText(line2, "Waktunya Pulang");
		goHome(NodeS);

	}
}

task main()
{
	for(int i=0; i<100; i++)
		Path[i] = 0;
	Stack NodeS;
	createEmpty(&NodeS);
	Stack S;
	createEmpty(&S);
	initialMove();
	DFS(S,NodeS);
}
