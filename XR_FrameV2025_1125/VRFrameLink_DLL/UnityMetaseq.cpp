#include "pch.h"
#include "UnityMetaseq.h"


static TEXTURE_POOL um_l_texPool[MAX_TEXTURE];		// テクスチャプール
static int			um_l_texPoolnum;				// テクスチャの数

static MQO_OBJECT* um_object = NULL;


void	u_mqoReadMaterial(FILE* fp, MQO_MATDATA M[]);
int		u_mqoLoadFile(MQO_OBJECT *mqoobj, char *filename, double scale, unsigned char alpha);
void	u_mqoDeleteModel(); 
void	u_mqoReadVertex(FILE *fp, umPOINT3f V[]);
int		u_mqoReadBVertex(FILE *fp, umPOINT3f V[]);
void	u_mqoReadFace(FILE *fp, MQO_FACE F[]);
int		u_mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha);
unsigned char* u_mqoLoadTextureEx(char *texfile, char *alpfile, int *tex_size, unsigned char alpha);

void u_mqoMakeObjectsEx(MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha);
umPOINT3f * u_mqoVertexNormal(MQO_OBJDATA *obj);
void u_mqoSnormal(umPOINT3f A, umPOINT3f B, umPOINT3f C, umPOINT3f *normal);
umPOINT3f * u_mqoVertexNormal(MQO_OBJDATA *obj);

void u_mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
	umPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha);

void u_mqoMakeArray(MQO_MATERIAL *mat, int matpos, MQO_FACE F[], int fnum, umPOINT3f V[],
	umPOINT3f N[], double facet, umCOLOR4f *mcol, double scale, unsigned char alpha);

void u_mqoGetDirectory(const char *path_file, char *path_dir);


inline void u_mqoCopyColor(float* src, umCOLOR4f* dst)
{
	dst->r = *(src + 0);
	dst->g = *(src + 1);
	dst->b = *(src + 2);
	dst->a = *(src + 3);
}

inline void u_mqoCopyColor(float* src, float* dst)
{
	*(dst + 0) = *(src + 0);
	*(dst + 1) = *(src + 1);
	*(dst + 2) = *(src + 2);
	*(dst + 3) = *(src + 3);
}



void u_mqoInit(void)
{
	// テクスチャプール初期化
	memset(um_l_texPool, 0, sizeof(um_l_texPool));
	um_l_texPoolnum = 0;
}

void u_mqoCleanup(void)
{
	// テクスチャプールの削除
	for (int i = 0; i < um_l_texPoolnum; i++)
	{
		if(um_l_texPool[i].image != NULL) free(um_l_texPool[i].image);
	}

	memset(um_l_texPool, 0, sizeof(um_l_texPool));
	um_l_texPoolnum = 0;
}

bool u_mqoCreateModel(char* filename, double scale)
{
	if (um_object != NULL) u_mqoDeleteModel();

	um_object = (MQO_OBJECT *)malloc(sizeof(MQO_OBJECT));
	memset(um_object, 0, sizeof(MQO_OBJECT));

	if (u_mqoLoadFile(um_object, filename, scale, (unsigned char)255) == 0) return false;

	return true;
}

int u_mqoObjectCount()
{
	if (um_object == NULL) return 0;
	else return um_object->objnum;
}

int u_mqoMaterialCount(int objectID)
{
	if (um_object == NULL) return 0;
	
	if (um_object->objnum <= objectID) return 0;

	return um_object->obj[objectID].matnum;
}

bool u_mqoMaterialInfo(int objectID, int materialID,
	int* isvalid, int* hastexture,
	float* dif, float* emi, float* spc,
	int* textureID
)
{
	if (um_object == NULL) return false;

	if (um_object->objnum <= objectID) return false;

	if (um_object->obj[objectID].matnum <= materialID) return false;

	MQO_MATERIAL* mat = &um_object->obj[objectID].mat[materialID];

	*isvalid = mat->isValidMaterialInfo;
	*hastexture = mat->isUseTexture;
	*textureID = mat->texture_id;
	u_mqoCopyColor(mat->dif, dif);
	u_mqoCopyColor(mat->emi, emi);
	u_mqoCopyColor(mat->spc, spc);
	

	return true;
}

int u_mqoArrayCount(int objectID, int materialID)
{
	if (um_object == NULL) return 0;

	if (um_object->objnum <= objectID) return 0;

	if (um_object->obj[objectID].matnum <= materialID) return 0;

	return um_object->obj[objectID].mat[materialID].u_arraynum;
}

bool u_mqoArrayInfo(int objectID, int materialID, float* vertex, float* normal, float* uv)
{
	if (um_object == NULL) return false;

	if (um_object->objnum <= objectID) return false;

	if (um_object->obj[objectID].matnum <= materialID) return false;

	MQO_MATERIAL* mat = &um_object->obj[objectID].mat[materialID];

	memcpy((void*)vertex, mat->u_vertex, sizeof(float)*mat->u_arraynum * 3);
	memcpy((void*)normal, mat->u_normal, sizeof(float)*mat->u_arraynum * 3);
	if(mat->u_uv != NULL)
		memcpy((void*)uv, mat->u_uv, sizeof(float)*mat->u_arraynum * 2);

	return true;
}

int  u_mqoTextureSize(int textureID)
{
	if (um_l_texPoolnum <= textureID) return 0;

	return um_l_texPool[textureID].texsize; //　正方形テクスチャのサイズ
}
bool u_mqoTextureImage(int textureID, unsigned char* image)
{
	if (um_l_texPoolnum <= textureID) return false;

	int size = um_l_texPool[textureID].texsize;

	memcpy((void*)image, um_l_texPool[textureID].image,
		sizeof(unsigned char) * size * size * 4);

	return true;
}

void u_mqoReadMaterial(FILE *fp, MQO_MATDATA M[])
{
	float		dif, amb, emi, spc;
	umCOLOR4f	c;
	char		buf[SIZE_STR];
	char		*pStrEnd, *pStr;
	int			len;
	int			i = 0;

	while (1) {
		fgets(buf, SIZE_STR, fp);	// 行読み込み
		if (strstr(buf, "}")) break;

		pStr = strstr(buf, "col(");	// 材質名読み飛ばし
		sscanf(pStr,
			"col(%f %f %f %f) dif (%f) amb(%f) emi(%f) spc(%f) power(%f)",
			&c.r, &c.g, &c.b, &c.a, &dif, &amb, &emi, &spc, &M[i].power);

		// 頂点カラー
		M[i].col = c;

		// 拡散光
		M[i].dif[0] = dif * c.r;
		M[i].dif[1] = dif * c.g;
		M[i].dif[2] = dif * c.b;
		M[i].dif[3] = c.a;

		// 周囲光
		M[i].amb[0] = amb * c.r;
		M[i].amb[1] = amb * c.g;
		M[i].amb[2] = amb * c.b;
		M[i].amb[3] = c.a;

		// 自己照明
		M[i].emi[0] = emi * c.r;
		M[i].emi[1] = emi * c.g;
		M[i].emi[2] = emi * c.b;
		M[i].emi[3] = c.a;

		// 反射光
		M[i].spc[0] = spc * c.r;
		M[i].spc[1] = spc * c.g;
		M[i].spc[2] = spc * c.b;
		M[i].spc[3] = c.a;

		// tex：模様マッピング名
		if ((pStr = strstr(buf, "tex(")) != NULL) {
			M[i].useTex = TRUE;

			pStrEnd = strstr(pStr, ")") - 1;
			len = pStrEnd - (pStr + 5);
			strncpy(M[i].texFile, pStr + 5, len);
			M[i].texFile[len] = (char)0;
			if ((pStr = strstr(buf, "aplane(")) != NULL) {
				pStrEnd = strstr(pStr, ")") - 1;
				len = pStrEnd - (pStr + 8);
				strncpy(M[i].alpFile, pStr + 8, len);
				M[i].alpFile[len] = (char)0;
			}
			else {
				M[i].alpFile[0] = (char)0;
			}

		}
		else {
			M[i].useTex = FALSE;
			M[i].texFile[0] = (char)0;
			M[i].alpFile[0] = (char)0;
		}

		i++;
	}

}

void u_mqoReadObject(FILE *fp, MQO_OBJDATA *obj)
{
	char buf[SIZE_STR];

	while (1) {
		fgets(buf, SIZE_STR, fp);
		if (strstr(buf, "}")) break;

		// visible
		if (strstr(buf, "visible ")) {
			sscanf(buf, " visible %d", &obj->visible);
		}

		// shading
		if (strstr(buf, "shading ")) {
			sscanf(buf, " shading %d", &obj->shading);
		}

		// facet
		if (strstr(buf, "facet ")) {
			sscanf(buf, " facet %f", &obj->facet);
		}

		// vertex
		if (strstr(buf, "vertex ")) {
			sscanf(buf, " vertex %d", &obj->n_vertex);
			obj->V = (umPOINT3f*)calloc(obj->n_vertex, sizeof(umPOINT3f));
			u_mqoReadVertex(fp, obj->V);
		}
		// BVertex
		if (strstr(buf, "BVertex")) {
			sscanf(buf, " BVertex %d", &obj->n_vertex);
			obj->V = (umPOINT3f*)calloc(obj->n_vertex, sizeof(umPOINT3f));
			u_mqoReadBVertex(fp, obj->V);
		}

		// face
		if (strstr(buf, "face ")) {
			sscanf(buf, " face %d", &obj->n_face);
			obj->F = (MQO_FACE*)calloc(obj->n_face, sizeof(MQO_FACE));
			u_mqoReadFace(fp, obj->F);
		}

	}
}

void u_mqoReadVertex(FILE *fp, umPOINT3f V[])
{
	char buf[SIZE_STR];
	int  i = 0;

	while (1) {
		fgets(buf, SIZE_STR, fp);
		if (strstr(buf, "}")) break;
		sscanf(buf, "%f %f %f", &V[i].x, &V[i].y, &V[i].z);
		i++;
	}
}

int u_mqoReadBVertex(FILE *fp, umPOINT3f V[])
{
	int n_vertex, i;
	float *wf;
	int size;
	char cw[256];
	char *pStr;

	fgets(cw, sizeof(cw), fp);
	if ((pStr = strstr(cw, "Vector")) != NULL) {
		sscanf(pStr, "Vector %d [%d]", &n_vertex, &size);	// 頂点数、データサイズを読み込む
	}
	else {
		return -1;
	}
	//MQOファイルのバイナリ頂点データはintel形式（リトルエディアン）
	wf = (float *)malloc(size);
	fread(wf, size, 1, fp);
	for (i = 0; i < n_vertex; i++) {
		V[i].x = wf[i * 3 + 0];
		V[i].y = wf[i * 3 + 1];
		V[i].z = wf[i * 3 + 2];
#if DEF_IS_LITTLE_ENDIAN
#else
		endianConverter((void *)&V[i].x, sizeof(V[i].x));
		endianConverter(&V[i].y, sizeof(V[i].y));
		endianConverter(&V[i].z, sizeof(V[i].z));
#endif
	}
	free(wf);

	// "}"まで読み飛ばし
	{
		char buf[SIZE_STR];
		while (1) {
			fgets(buf, SIZE_STR, fp);
			if (strstr(buf, "}")) break;
		}
	}

	return n_vertex;
}

void u_mqoReadFace(FILE *fp, MQO_FACE F[])
{
	char buf[SIZE_STR];
	char *pStr;
	int  i = 0;

	while (1) {
		fgets(buf, SIZE_STR, fp);
		if (strstr(buf, "}")) break;

		// 面を構成する頂点数
		sscanf(buf, "%d", &F[i].n);

		// 頂点(V)の読み込み
		if ((pStr = strstr(buf, "V(")) != NULL) {
			switch (F[i].n) {
			case 3:
				//メタセコは頂点の並びが表面からみて右回り
				//読み込み時に並べ替える方法もある。けど、表面の設定を
				//glFrontFaceで変えるほうがスマート？
				sscanf(pStr, "V(%d %d %d)", &F[i].v[0], &F[i].v[1], &F[i].v[2]);
				//					sscanf(pStr,"V(%d %d %d)",&F[i].v[2],&F[i].v[1],&F[i].v[0]);
				break;
			case 4:
				sscanf(pStr, "V(%d %d %d %d)", &F[i].v[0], &F[i].v[1], &F[i].v[2], &F[i].v[3]);
				//					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[3],&F[i].v[2],&F[i].v[1],&F[i].v[0]);
				break;
			default:
				break;
			}
		}

		// マテリアル(M)の読み込み
		F[i].m = 0;
		if ((pStr = strstr(buf, "M(")) != NULL) {
			sscanf(pStr, "M(%d)", &F[i].m);
		}
		else { // マテリアルが設定されていない面
			F[i].m = -1;
		}

		// UVマップ(UV)の読み込み
		if ((pStr = strstr(buf, "UV(")) != NULL) {
			switch (F[i].n) {
			case 3:	// 頂点数3
				sscanf(pStr, "UV(%f %f %f %f %f %f)",
					&F[i].uv[0].x, &F[i].uv[0].y,
					&F[i].uv[1].x, &F[i].uv[1].y,
					&F[i].uv[2].x, &F[i].uv[2].y
				);
				break;

			case 4:	// 頂点数4
				sscanf(pStr, "UV(%f %f %f %f %f %f %f %f)",
					&F[i].uv[0].x, &F[i].uv[0].y,
					&F[i].uv[1].x, &F[i].uv[1].y,
					&F[i].uv[2].x, &F[i].uv[2].y,
					&F[i].uv[3].x, &F[i].uv[3].y
				);
				break;
			default:
				break;
			}
		}

		i++;
	}

}

int u_mqoLoadFile(MQO_OBJECT *mqoobj, char *filename, double scale, unsigned char alpha)
{
	FILE			*fp;
	MQO_OBJDATA		obj[MAX_OBJECT];
	MQO_MATDATA		*M = NULL;

	char	buf[SIZE_STR];		// 文字列読み込みバッファ
	char	path_dir[SIZE_STR];	// ディレクトリのパス
	char	path_tex[SIZE_STR];	// テクスチャファイルのパス
	char	path_alp[SIZE_STR];	// アルファテクスチャファイルのパス
	int		n_mat = 0;			// マテリアル数
	int		n_obj = 0;			// オブジェクト数
	int		i;

	// MaterialとObjectの読み込み
	fp = fopen(filename, "rb");
	if (fp == NULL) return 0;

	mqoobj->alpha = alpha;
	memset(obj, 0, sizeof(obj));

	i = 0;
	while (!feof(fp)) {
		fgets(buf, SIZE_STR, fp);

		// Material
		if (strstr(buf, "Material")) {
			sscanf(buf, "Material %d", &n_mat);
			M = (MQO_MATDATA*)calloc(n_mat, sizeof(MQO_MATDATA));
			u_mqoReadMaterial(fp, M);
		}

		// Object
		if (strstr(buf, "Object")) {
			sscanf(buf, "Object %s", obj[i].objname);
			u_mqoReadObject(fp, &obj[i]);
			i++;
		}
	}
	n_obj = i;
	fclose(fp);

	// パスの取得
	u_mqoGetDirectory(filename, path_dir);

	// テクスチャの登録
	for (i = 0; i < n_mat; i++) {
		if (M[i].useTex) {

			if (strstr(M[i].texFile, ":")) {
				strcpy(path_tex, M[i].texFile);	// 絶対パスの場合
			}
			else {
				sprintf(path_tex, "%s%s", path_dir, M[i].texFile);	// 相対パスの場合
			}

			if (M[i].alpFile[0] != (char)0) {
				if (strstr(M[i].texFile, ":")) {
					strcpy(path_alp, M[i].alpFile);	// 絶対パスの場合
				}
				else {
					sprintf(path_alp, "%s%s", path_dir, M[i].alpFile);	// 相対パスの場合
				}
				M[i].texID = u_mqoSetTexturePool(path_tex, path_alp, alpha);
			}
			else {
				M[i].texID = u_mqoSetTexturePool(path_tex, NULL, alpha);
			}
		}
	}

	u_mqoMakeObjectsEx(mqoobj, obj, n_obj, M, n_mat, scale, alpha);

	// オブジェクトのデータの開放
	for (i = 0; i < n_obj; i++) {
		free(obj[i].V);
		free(obj[i].F);
	}

	// マテリアルの開放
	free(M);

	return 1;
}

void u_mqoGetDirectory(const char *path_file, char *path_dir)
{
	const char *pStr;
	int len;

	pStr = MAX(strrchr(path_file, '\\'), strrchr(path_file, '/'));
	len = MAX((int)(pStr - path_file) + 1, 0);
	strncpy(path_dir, path_file, len);
	path_dir[len] = (char)0;
}

int u_mqoSetTexturePool(char *texfile, char *alpfile, unsigned char alpha)
{
	int pos;
	unsigned char *image;

	for (pos = 0; pos < um_l_texPoolnum; pos++) {
		if (alpha != um_l_texPool[pos].alpha) {
			continue;
		}
		if (texfile != NULL) {
			if (strcmp(texfile, um_l_texPool[pos].texfile) != 0) {
				continue;
			}
		}
		if (alpfile != NULL) {
			if (strcmp(alpfile, um_l_texPool[pos].alpfile) != 0) {
				continue;
			}
		}
		break;
	}
	if (pos < um_l_texPoolnum) { //すでに読み込み済み
		return  um_l_texPool[pos].texture_id;
	}
	if (MAX_TEXTURE <= pos) {
		printf("%s:mqoSetTexturePool テクスチャ読み込み領域不足\n", __FILE__);
		return -1;
	}
	image = u_mqoLoadTextureEx(texfile, alpfile, &um_l_texPool[pos].texsize, alpha);
	if (image == NULL) {
		return -1;
	}
	um_l_texPool[pos].image = image;

	if (texfile != NULL) strncpy(um_l_texPool[pos].texfile, texfile, MAX_PATH);
	if (alpfile != NULL) strncpy(um_l_texPool[pos].alpfile, alpfile, MAX_PATH);
	um_l_texPool[pos].alpha = alpha;

	um_l_texPoolnum = pos + 1;

	//登録すれば、読み込んだバッファは不要
	//free(image);

	um_l_texPool[pos].texture_id = pos; // list ID == texture ID

	return um_l_texPool[pos].texture_id;
}

unsigned char* u_mqoLoadTextureEx(char *texfile, char *alpfile, int *tex_size, unsigned char alpha)
{
	FILE *fp;
	size_t namelen;
	char ext[4];
	char wbuf[3];
	int isTGA;
	int isPNG;
	int isJPEG;
	int other;
	int	y, x, size;
	int fl;
	char *filename[2];
	int width[2];
	int sts;
	STR_TGA_HEAD tgah;
	unsigned char	*pImage, *pRead;

#if DEF_USE_LIBJPEG
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY jpegimage;
#endif
#if DEF_USE_LIBPNG
	unsigned char **pngimage;
	unsigned long   pngwidth, pngheight;
	int				pngdepth;
	int             color_type;
#endif

	filename[0] = texfile;
	filename[1] = alpfile;
	width[0] = -1;
	width[1] = -1;
	pImage = NULL;
	fp = NULL;
	sts = 0;
#if DEF_USE_LIBJPEG
	jpegimage = NULL;
#endif
#if DEF_USE_LIBPNG
	pngimage = NULL;
#endif
	size = -1;
	for (fl = 0; fl < 2; fl++) {//テクスチャ＝fl=0    アルファ＝fl=1
		if (filename[fl] == NULL) continue;
		namelen = strlen(filename[fl]);
		ext[0] = tolower(filename[fl][namelen - 3]);
		ext[1] = tolower(filename[fl][namelen - 2]);
		ext[2] = tolower(filename[fl][namelen - 1]);
		ext[3] = 0x00;
		isTGA = (strcmp(ext, "tga") == 0) ? 1 : 0;
		isPNG = (strcmp(ext, "png") == 0) ? 1 : 0;
		isJPEG = (strcmp(ext, "jpg") == 0) ? 1 : 0;
		/* */
		if ((!isTGA) && (!isPNG) && (!isJPEG)) {
			filename[fl][namelen - 3] = 'b';
			filename[fl][namelen - 2] = 'm';
			filename[fl][namelen - 1] = 'p';
		}
		/* */
		if (fl == 1) { //アルファの読み込みはＴＧＡorＰＮＧ
			if (!(isTGA || isPNG)) {
				printf("アルファのファイルに対応できない→%s\n", filename[fl]);
				break;
			}
		}
		if (fp != NULL) fclose(fp);
		if ((fp = fopen(filename[fl], "rb")) == NULL) {
			printf("%s:テクスチャ読み込みエラー[%s]\n", __FILE__, filename[fl]);
			continue;
		}
		// ヘッダのロード
		if (isTGA) {
			fread(&tgah, sizeof(STR_TGA_HEAD), 1, fp);
#if DEF_IS_LITTLE_ENDIAN
#else
			TGAHeaderEndianConverter(&tgah);
#endif
			size = width[fl] = tgah.width;
		}
		if (isJPEG) {
#if DEF_USE_LIBJPEG
			unsigned int i;
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);	//解凍用情報作成
			jpeg_stdio_src(&cinfo, fp);		//読み込みファイル指定
			jpeg_read_header(&cinfo, TRUE);	//jpegヘッダ読み込み
			jpeg_start_decompress(&cinfo);	//解凍開始

			if (cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB) {
				if (jpegimage != NULL) {
					for (i = 0; i < cinfo.output_height; i++) free(jpegimage[i]);            // 以下２行は２次元配列を解放します
					free(jpegimage);
				}
				//読み込みデータ配列の作成
				jpegimage = (JSAMPARRAY)malloc(sizeof(JSAMPROW) * cinfo.output_height);
				for (i = 0; i < cinfo.output_height; i++) {
					jpegimage[i] = (JSAMPROW)malloc(sizeof(JSAMPLE) * cinfo.out_color_components * cinfo.output_width);
				}
				//解凍データ読み込み
				while (cinfo.output_scanline < cinfo.output_height) {
					jpeg_read_scanlines(&cinfo,
						jpegimage + cinfo.output_scanline,
						cinfo.output_height - cinfo.output_scanline
					);
				}
				size = width[fl] = cinfo.output_width;
			}

			jpeg_finish_decompress(&cinfo);	//解凍終了
			jpeg_destroy_decompress(&cinfo);	//解凍用情報解放
			if (!(cinfo.out_color_components == 3 && cinfo.out_color_space == JCS_RGB)) {
				printf("JPEG 対応できないフォーマット→%s\n", filename[fl]);
			}
#else
			printf("このテクスチャは対応できないフォーマット→%s\n", filename[fl]);
			continue;
#endif
		}
		if (isPNG) {
#if DEF_USE_LIBPNG
			png_structp     png_ptr;
			png_infop       info_ptr;
			int             bit_depth, interlace_type;
			unsigned int             i;
			int j, k;
			png_ptr = png_create_read_struct(                       // png_ptr構造体を確保・初期化します
				PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
			info_ptr = png_create_info_struct(png_ptr);             // info_ptr構造体を確保・初期化します
			png_init_io(png_ptr, fp);                               // libpngにfpを知らせます
			png_read_info(png_ptr, info_ptr);                       // PNGファイルのヘッダを読み込みます
			png_get_IHDR(png_ptr, info_ptr, &pngwidth, &pngheight,        // IHDRチャンク情報を取得します
				&bit_depth, &color_type, &interlace_type,
				&j, &k);
			if (pngimage != NULL) {
				for (i = 0; i < pngheight; i++) free(pngimage[i]);            // 以下２行は２次元配列を解放します
				free(pngimage);
			}
			pngimage = (png_bytepp)malloc(pngheight * sizeof(png_bytep)); // 以下３行は２次元配列を確保します
			i = png_get_rowbytes(png_ptr, info_ptr);
			pngdepth = i / pngwidth;
			for (i = 0; i < pngheight; i++)
				pngimage[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
			png_read_image(png_ptr, pngimage);                         // 画像データを読み込みます

			png_destroy_read_struct(                                // ２つの構造体のメモリを解放します
				&png_ptr, &info_ptr, (png_infopp)NULL);
			size = width[fl] = pngwidth;
#else
			printf("このテクスチャは対応できないフォーマット→%s\n", filename[fl]);
			continue;
#endif
		}
		if (width[fl] == -1) {//ココまできてサイズが指定されていない　＝　ビットマップ
			fseek(fp, 14 + 4, SEEK_SET);		// 画像幅が格納されている位置までシーク
			fread(&size, sizeof(int), 1, fp);	// BiWidthの情報だけ取得
			fseek(fp, 14 + 40, SEEK_SET);		// 画素データが格納されている位置までシーク
#if DEF_IS_LITTLE_ENDIAN
#else
			endianConverter(&size, sizeof(int));
#endif
			width[fl] = size;

		}
		if (width[0] != -1 && width[1] != -1) {
			if (width[0] != width[1]) {
				sts = -1;
				break;
			}
		}
		if (fl == 1 && isTGA) { //アルファの読み込みはＴＧＡの８ビットモノクロｏｒ３２ビットフル
			if (!(
				(tgah.depth == 8 && tgah.type == DEF_TGA_TYPE_MONO) ||
				(tgah.depth == 32 && tgah.type == DEF_TGA_TYPE_FULL)
				)) {
				break;
			}
		}
		if (fl == 1 && isPNG) { //アルファの読み込みはＰＮＧのトゥルーカラー＋アルファｏｒグレースケール＋アルファ
#if DEF_USE_LIBPNG
			if (!(
				(color_type == 6) ||
				(color_type == 4)
				)) {
				break;
			}
#endif
		}

		// メモリの確保
		if (pImage == NULL) {
			pImage = (unsigned char*)malloc(sizeof(unsigned char)*size*size * 4);
		}
		if (pImage == NULL) return NULL;
		for (y = 0; y < size; y++) {
			pRead = pImage + (size - 1 - y) * 4 * size;
			for (x = 0; x < size; x++) {
				other = 1;
				if (fl == 0) {
					if (isJPEG) {
#if DEF_USE_LIBJPEG
						pRead[0] = jpegimage[size - 1 - y][x * 3];
						pRead[1] = jpegimage[size - 1 - y][x * 3 + 1];
						pRead[2] = jpegimage[size - 1 - y][x * 3 + 2];
						pRead[3] = alpha;				// A
						other = 0;
#endif
					}
					if (isPNG) {
#if DEF_USE_LIBPNG
						if (color_type == 2 || color_type == 6) {
							pRead[0] = pngimage[size - 1 - y][x*pngdepth];
							pRead[1] = pngimage[size - 1 - y][x*pngdepth + 1];
							pRead[2] = pngimage[size - 1 - y][x*pngdepth + 2];
							pRead[3] = alpha;				// A
							if (color_type == 6) {
								pRead[3] = pngimage[size - 1 - y][x*pngdepth + 3];
							}
						}
						other = 0;
#endif
					}
					if (other) {
						fread(&pRead[2], 1, 1, fp);	// B
						fread(&pRead[1], 1, 1, fp);	// G	
						fread(&pRead[0], 1, 1, fp);	// R
						pRead[3] = alpha;				// A
						if (isTGA && tgah.depth == 32) {
							fread(&pRead[3], 1, 1, fp);	// A
							if (alpha < pRead[3]) pRead[3] = alpha;
						}
					}
				}
				else {
					if (isPNG) {
#if DEF_USE_LIBPNG
						if (color_type == 6) { //トゥルーカラー＋アルファ
							pRead[3] = pngimage[size - 1 - y][x*pngdepth + 3];
						}
						if (color_type == 4) { //グレースケール＋アルファ
							pRead[3] = pngimage[size - 1 - y][x*pngdepth + 1];
						}
						if (alpha < pRead[3]) pRead[3] = alpha;
#endif
					}
					if (isTGA) {
						if (tgah.depth == 32) { //いらないデータを読み飛ばす
							fread(wbuf, 3, 1, fp);	// BGR
						}
						fread(&pRead[3], 1, 1, fp);	// A
						if (alpha < pRead[3]) pRead[3] = alpha;
					}
				}
				pRead += 4;
			}
		}
		fclose(fp);
		fp = NULL;
	}
	if (sts != 0) {
		if (pImage != NULL) free(pImage);
		if (fp != NULL) fclose(fp);
	}
#if DEF_USE_LIBPNG
	if (pngimage != NULL) {
		unsigned int uy;
		for (uy = 0; uy < pngheight; uy++) free(pngimage[uy]);            // 以下２行は２次元配列を解放します
		free(pngimage);
	}
#endif
#if DEF_USE_LIBJPEG
	if (jpegimage != NULL) {
		unsigned int uy;
		for (uy = 0; uy < cinfo.output_height; uy++) free(jpegimage[uy]);            // 以下２行は２次元配列を解放します
		free(jpegimage);
	}
#endif
	if (size < 0) {
		if (pImage != NULL) free(pImage);
		pImage = NULL;
	}
	*tex_size = size;

	return pImage;
}

void u_mqoMakeObjectsEx(MQO_OBJECT *mqoobj, MQO_OBJDATA obj[], int n_obj, MQO_MATDATA M[], int n_mat,
	double scale, unsigned char alpha)
{
	int i;
	umPOINT3f *N;
	for (i = 0; i < n_obj; i++) {
		N = u_mqoVertexNormal(&obj[i]);
		u_mqoMakePolygon(
			&obj[i],
			mqoobj,
			N,
			M, n_mat,
			scale,
			alpha);
		free(N);
	}
}

umPOINT3f * u_mqoVertexNormal(MQO_OBJDATA *obj)
{
	int f;
	int v;
	int i;
	double len;
	umPOINT3f fnormal;	// 面法線ベクトル
	MQO_FACE *F;
	umPOINT3f *V;
	umPOINT3f *ret;
	F = obj->F;
	V = obj->V;
	ret = (umPOINT3f *)calloc(obj->n_vertex, sizeof(umPOINT3f));
	//面の法線を頂点に足し込み
	for (f = 0; f < obj->n_face; f++) {
		if (obj->F[f].n == 3) {
			u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &fnormal);
			for (i = 0; i < 3; i++) {
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
		if (obj->F[f].n == 4) {
			u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &fnormal);
			for (i = 0; i < 4; i++) {
				if (i == 3) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
			u_mqoSnormal(V[F[f].v[0]], V[F[f].v[2]], V[F[f].v[3]], &fnormal);
			for (i = 0; i < 4; i++) {
				if (i == 1) continue;
				ret[F[f].v[i]].x += fnormal.x;
				ret[F[f].v[i]].y += fnormal.y;
				ret[F[f].v[i]].z += fnormal.z;
			}
		}
	}
	//正規化
	for (v = 0; v < obj->n_vertex; v++) {
		if (ret[v].x == 0 && ret[v].y == 0 && ret[v].z == 0) {
			//面に使われてない点
			continue;
		}
		len = sqrt(ret[v].x*ret[v].x + ret[v].y*ret[v].y + ret[v].z*ret[v].z);
		if (len != 0) {
			ret[v].x = ret[v].x / len;
			ret[v].y = ret[v].y / len;
			ret[v].z = ret[v].z / len;
		}
	}

	return ret;
}

void u_mqoSnormal(umPOINT3f A, umPOINT3f B, umPOINT3f C, umPOINT3f *normal)
{
	double norm;
	umPOINT3f vec0, vec1;

	// ベクトルBA
	vec0.x = A.x - B.x;
	vec0.y = A.y - B.y;
	vec0.z = A.z - B.z;

	// ベクトルBC
	vec1.x = C.x - B.x;
	vec1.y = C.y - B.y;
	vec1.z = C.z - B.z;

	// 法線ベクトル
	normal->x = vec0.y * vec1.z - vec0.z * vec1.y;
	normal->y = vec0.z * vec1.x - vec0.x * vec1.z;
	normal->z = vec0.x * vec1.y - vec0.y * vec1.x;

	// 正規化
	norm = normal->x * normal->x + normal->y * normal->y + normal->z * normal->z;
	norm = sqrt(norm);

	normal->x /= (float)norm;
	normal->y /= (float)norm;
	normal->z /= (float)norm;
}

void u_mqoMakePolygon(MQO_OBJDATA *readObj, MQO_OBJECT *mqoobj,
	umPOINT3f N[], MQO_MATDATA M[], int n_mat, double scale, unsigned char alpha)
{

	MQO_INNER_OBJECT		*setObj;
	MQO_MATERIAL			*material;
	umCOLOR4f				defcol;
	umCOLOR4f				*pcol;
	int						f, m, *mat_vnum;
	int						fnum;
	MQO_FACE				*F;
	umPOINT3f				*V;
	double					facet;


	setObj = &mqoobj->obj[mqoobj->objnum];
	strcpy(setObj->objname, readObj->objname);
	setObj->isVisible = readObj->visible;
	setObj->isShadingFlat = (readObj->shading == 0);
	F = readObj->F;
	fnum = readObj->n_face;
	V = readObj->V;
	facet = readObj->facet;

	// faceの中でのマテリアル毎の頂点の数
	// M=NULLのとき、F[].m = 0 が入ってくる
	if (M == NULL) n_mat = 1;

	mat_vnum = (int *)malloc(sizeof(int)*n_mat);
	memset(mat_vnum, 0, sizeof(int)*n_mat);

	for (f = 0; f < fnum; f++) {
		if (F[f].m < 0) continue; // マテリアルが設定されていない面
		if (F[f].n == 3) {
			mat_vnum[F[f].m] += 3;
		}
		if (F[f].n == 4) {
			//４頂点（四角）は３頂点（三角）ｘ２に分割
			//  0  3      0    0  3
			//   □   →　△　　▽
			//  1  2     1  2   2
			// ４頂点の平面データは
			// ３頂点の平面データｘ２個
			mat_vnum[F[f].m] += 3 * 2;
		}
		if (setObj->matnum < F[f].m + 1) setObj->matnum = F[f].m + 1;
	}

	// マテリアル別に頂点配列を作成する
	setObj->mat = (MQO_MATERIAL *)malloc(sizeof(MQO_MATERIAL)*setObj->matnum);
	memset(setObj->mat, 0, sizeof(MQO_MATERIAL)*setObj->matnum);

	for (m = 0; m < setObj->matnum; m++) {
		material = &setObj->mat[m];
		material->datanum = mat_vnum[m];
		material->isValidMaterialInfo = (M != NULL);

		if (mat_vnum[m] <= 0) continue;
		if (material->isValidMaterialInfo) {
			memcpy(material->dif, M[m].dif, sizeof(material->dif));
			memcpy(material->amb, M[m].amb, sizeof(material->amb));
			memcpy(material->spc, M[m].spc, sizeof(material->spc));
			memcpy(material->emi, M[m].emi, sizeof(material->emi));
			material->power = M[m].power;
			material->isUseTexture = M[m].useTex;
			pcol = &M[m].col;
		}
		else {
			defcol.r = 1.0;
			defcol.g = 1.0;
			defcol.b = 1.0;
			defcol.a = 1.0;
			material->isUseTexture = 0;
			pcol = &defcol;
		}
		if (material->isUseTexture) {
			material->vertex_t = (VERTEX_TEXUSE *)calloc(material->datanum, sizeof(VERTEX_TEXUSE));
			material->texture_id = M[m].texID;
		}
		else {
			material->vertex_p = (VERTEX_NOTEX *)calloc(material->datanum, sizeof(VERTEX_NOTEX));
		}
		u_mqoMakeArray(material, m, F, fnum, V, N, facet, pcol, scale, alpha);
		//if (g_isVBOSupported) {
		//	if (material->isUseTexture) {
		//		glGenBuffersARB(1, &material->VBO_id);
		//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, material->VBO_id);
		//		glBufferDataARB(GL_ARRAY_BUFFER_ARB, material->datanum * sizeof(VERTEX_TEXUSE), material->vertex_t, GL_STATIC_DRAW_ARB);
		//	}
		//	else {
		//		glGenBuffersARB(1, &material->VBO_id);
		//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, material->VBO_id);
		//		glBufferDataARB(GL_ARRAY_BUFFER_ARB, material->datanum * sizeof(VERTEX_NOTEX), material->vertex_p, GL_STATIC_DRAW_ARB);
		//	}
		//}
	}
	mqoobj->objnum++;
	if (MAX_OBJECT <= mqoobj->objnum) {
		printf("MQOファイル読み込み：　最大オブジェクト数を超えました[%d]\n", mqoobj->objnum);
		mqoobj->objnum = MAX_OBJECT - 1;
	}

	free(mat_vnum);

}

void u_mqoMakeArray(MQO_MATERIAL *mat, int matpos, MQO_FACE F[], int fnum, umPOINT3f V[],
	umPOINT3f N[], double facet, umCOLOR4f *mcol, double scale, unsigned char alpha)
{
	int f;
	int i;
	int dpos;
	double s;
	umPOINT3f normal;	// 法線ベクトル

	dpos = 0;
	mat->color[0] = mcol->r;
	mat->color[1] = mcol->g;
	mat->color[2] = mcol->b;
	mat->color[3] = mcol->a;
	if (mat->isUseTexture) {
		for (f = 0; f < fnum; f++) {
			if (F[f].m != matpos) continue;
			if (F[f].n == 3) {
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &normal);	// 法線ベクトルを計算
				for (i = 0; i < 3; i++) {
					mat->vertex_t[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						// スムージング角　＜（頂点法線と面法線の角度）のときは面法線を頂点法線とする
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//４頂点（四角）は３頂点（三角）ｘ２に分割
			if (F[f].n == 4) {
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &normal);	// 法線ベクトルを計算
				for (i = 0; i < 4; i++) {
					if (i == 3) continue;
					mat->vertex_t[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[2]], V[F[f].v[3]], &normal);	// 法線ベクトルを計算
				for (i = 0; i < 4; i++) {
					if (i == 1) continue;
					mat->vertex_t[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_t[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_t[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_t[dpos].uv[0] = F[f].uv[i].x;
					mat->vertex_t[dpos].uv[1] = F[f].uv[i].y;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						mat->vertex_t[dpos].normal[0] = normal.x;
						mat->vertex_t[dpos].normal[1] = normal.y;
						mat->vertex_t[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_t[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_t[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_t[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}
	else {
		if (alpha != 255) {
			mat->color[3] = (float)((double)alpha / (double)255);
		}
		for (f = 0; f < fnum; f++) {
			if (F[f].m != matpos) continue;
			if (F[f].n == 3) {
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &normal);		// 法線ベクトルを計算
				for (i = 0; i < 3; i++) {
					mat->vertex_p[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
			//４頂点（四角）は３頂点（三角）ｘ２に分割
			if (F[f].n == 4) {
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[1]], V[F[f].v[2]], &normal);		// 法線ベクトルを計算
				for (i = 0; i < 4; i++) {
					if (i == 3) continue;
					mat->vertex_p[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
				u_mqoSnormal(V[F[f].v[0]], V[F[f].v[2]], V[F[f].v[3]], &normal);		// 法線ベクトルを計算
				for (i = 0; i < 4; i++) {
					if (i == 1) continue;
					mat->vertex_p[dpos].point[0] = (float)V[F[f].v[i]].x*scale;
					mat->vertex_p[dpos].point[1] = (float)V[F[f].v[i]].y*scale;
					mat->vertex_p[dpos].point[2] = (float)V[F[f].v[i]].z*scale;
					mat->vertex_p[dpos].normal[0] = normal.x;
					mat->vertex_p[dpos].normal[1] = normal.y;
					mat->vertex_p[dpos].normal[2] = normal.z;
					s = acos(normal.x*N[F[f].v[i]].x + normal.y*N[F[f].v[i]].y + normal.z*N[F[f].v[i]].z);
					if (facet < s) {
						mat->vertex_p[dpos].normal[0] = normal.x;
						mat->vertex_p[dpos].normal[1] = normal.y;
						mat->vertex_p[dpos].normal[2] = normal.z;
					}
					else {
						mat->vertex_p[dpos].normal[0] = N[F[f].v[i]].x;
						mat->vertex_p[dpos].normal[1] = N[F[f].v[i]].y;
						mat->vertex_p[dpos].normal[2] = N[F[f].v[i]].z;
					}
					dpos++;
				}
			}
		}
	}

	// for Unity Array
	mat->u_vertex = (float*)malloc(sizeof(float) * dpos * 3);
	mat->u_normal = (float*)malloc(sizeof(float) * dpos * 3);
	mat->u_arraynum = dpos;

	if (mat->vertex_t != NULL)
	{
		if (mat->vertex_t != NULL) mat->u_uv = (float*)malloc(sizeof(float) * dpos * 2);

		for (int i = 0; i < dpos; i++)
		{
			mat->u_vertex[i * 3 + 0] = mat->vertex_t[i].point[0];
			mat->u_vertex[i * 3 + 1] = mat->vertex_t[i].point[1];
			mat->u_vertex[i * 3 + 2] = mat->vertex_t[i].point[2];
			mat->u_normal[i * 3 + 0] = mat->vertex_t[i].normal[0];
			mat->u_normal[i * 3 + 1] = mat->vertex_t[i].normal[1];
			mat->u_normal[i * 3 + 2] = mat->vertex_t[i].normal[2];
			mat->u_uv[i * 2 + 0] = mat->vertex_t[i].uv[0];
			mat->u_uv[i * 2 + 1] = mat->vertex_t[i].uv[1];
		}
	}
	else
	{
		for (int i = 0; i < dpos; i++)
		{
			mat->u_vertex[i * 3 + 0] = mat->vertex_p[i].point[0];
			mat->u_vertex[i * 3 + 1] = mat->vertex_p[i].point[1];
			mat->u_vertex[i * 3 + 2] = mat->vertex_p[i].point[2];
			mat->u_normal[i * 3 + 0] = mat->vertex_p[i].normal[0];
			mat->u_normal[i * 3 + 1] = mat->vertex_p[i].normal[1];
			mat->u_normal[i * 3 + 2] = mat->vertex_p[i].normal[2];
		}
	}
}

void u_mqoClearObject(MQO_OBJECT object[], int from, int num)
{
	int					loop, o, m;
	MQO_INNER_OBJECT	*obj;
	MQO_MATERIAL		*mat;

	if (object == NULL) return;

	for (loop = from; loop < from + num; loop++) {
		for (o = 0; o < (object + from)->objnum; o++) {

			obj = &(object + loop)->obj[o];
			for (m = 0; m < obj->matnum; m++) {
				mat = &obj->mat[m];
				if (mat->datanum <= 0) continue;
				//if (g_isVBOSupported) {
				//	// 頂点バッファの削除
				//	glDeleteBuffersARB(1, &mat->VBO_id);
				//}

				// 頂点配列の削除
				if (mat->isUseTexture) {
					if (mat->vertex_t != NULL) {
						free(mat->vertex_t);
						mat->vertex_t = NULL;
					}
				}
				else {
					if (mat->vertex_p != NULL) {
						free(mat->vertex_p);
						mat->vertex_p = NULL;
					}
				}
				if (mat->u_vertex != NULL)
				{
					free(mat->u_vertex);
					mat->u_vertex = NULL;
				}
				if (mat->u_normal != NULL)
				{
					free(mat->u_normal);
					mat->u_normal = NULL;
				}
				if (mat->u_uv != NULL)
				{
					free(mat->u_uv);
					mat->u_uv = NULL;
				}
			}
			if (obj->mat != NULL) {
				free(obj->mat);
				obj->mat = NULL;
			}
			obj->matnum = 0;
		}
	}

}


void u_mqoDeleteObject(MQO_OBJECT object[], int num)
{
	u_mqoClearObject(object, 0, num);
	free(object);
}


void u_mqoDeleteModel()
{
	if(um_object != NULL) 
		u_mqoDeleteObject(um_object, 1);

	um_object = NULL;
}
