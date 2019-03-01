#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define STATUS_SUCCESS	0
#define STATUS_ERR		1

#define STATE_IN_BOARD			1
#define STATE_OUT_OF_BOARD		2
#define STATE_EDGE_OF_BOARD		4
#define STATE_ARRIVED_BEFORE	8

#define STATE_IS(state, mask) (state & mask)
#define ARRAY_GET(array, col_count, row, col)				(array[col_count * (row - 1) + (col - 1)])
#define ARRAY_GET_POINTER(array, col_count, row, col)		(&(array[col_count * (row - 1) + (col - 1)]))

typedef struct Pos {
	int x;
	int y;
} Pos, *pPos;

int checkHorseState(pPos cboardSet, pPos pHorse) {
	if (
		pHorse->x < 1 || pHorse->y < 1 ||
		pHorse->x > cboardSet->x || pHorse->y > cboardSet->y
		) {
		return STATE_OUT_OF_BOARD;
	}

	if (
		pHorse->x == 1 || pHorse->x == cboardSet->x ||
		pHorse->y == 1 || pHorse->y == cboardSet->y
		) {
		return STATE_EDGE_OF_BOARD | STATE_IN_BOARD;
	}

	return STATE_IN_BOARD;
}

int printChessBoard(pPos cboardSet, pPos pHorse) {
	int state = checkHorseState(cboardSet, pHorse);
	if (state == STATE_OUT_OF_BOARD) {
		return STATUS_ERR;
	}

	for (unsigned int i = cboardSet->y; i >= 1; i--) {
		printf_s(" %u ", i);
		for (unsigned int j = 1; j <= cboardSet->x; j++) {
			if (i == pHorse->y && j == pHorse->x) {
				printf_s(" A ");
			}
			else {
				printf_s(" . ");
			}
		}
		printf_s("\r\n");
	}
	printf_s("y/x");
	for (unsigned int i = cboardSet->x; i >= 1; i--) {
		printf_s(" %u ", cboardSet->x - i + 1);
	}
	printf_s("\r\n");

	return STATUS_SUCCESS;
}

int printChessStep(pPos cboardSet, int *chessStep) {
	for (unsigned int i = cboardSet->y; i >= 1; i--) {
		printf_s(" %3u ", i);
		for (unsigned int j = 1; j <= cboardSet->x; j++) {
			if (ARRAY_GET(chessStep, cboardSet->x, i, j)) {
				printf_s(" %3d ", ARRAY_GET(chessStep, cboardSet->x, i, j));
			}
			else {
				printf_s("   . ");
			}
		}
		printf_s("\r\n");
	}
	printf_s(" y/x ");
	for (unsigned int i = cboardSet->x; i >= 1; i--) {
		printf_s(" %3u ", cboardSet->x - i + 1);
	}
	printf_s("\r\n");

	return STATUS_SUCCESS;
}

int moveChess(pPos cboardSet, pPos pHorseInit, pPos pHorseCurrnet, int *chessStep, bool edgeReached, int step) {
	if (ARRAY_GET(chessStep, cboardSet->x, pHorseCurrnet->y, pHorseCurrnet->x) > 1) {
		return STATE_ARRIVED_BEFORE;
	}

	if (STATE_IS(checkHorseState(cboardSet, pHorseCurrnet), STATE_OUT_OF_BOARD)) {
		return STATE_OUT_OF_BOARD;
	}

	if (STATE_IS(checkHorseState(cboardSet, pHorseCurrnet), STATE_EDGE_OF_BOARD)) {
		edgeReached = true;
	}

	int *chessStepOri = chessStep;
	chessStep = malloc(cboardSet->x * cboardSet->y * sizeof(int));
	memcpy(chessStep, chessStepOri, cboardSet->x * cboardSet->y * sizeof(int));
	*(ARRAY_GET_POINTER(chessStep, cboardSet->x, pHorseCurrnet->y, pHorseCurrnet->x)) = step;

	if (
		pHorseCurrnet->x == pHorseInit->x &&
		pHorseCurrnet->y == pHorseInit->y &&
		edgeReached
		) {
		*(ARRAY_GET_POINTER(chessStep, cboardSet->x, pHorseCurrnet->y, pHorseCurrnet->x)) = 1;
		printf_s("ok:\r\n");
		printChessStep(cboardSet, chessStep);
		printf_s("\r\n");
		return STATUS_SUCCESS;
	}

	Pos newHorseCurrent;
	newHorseCurrent.x = pHorseCurrnet->x + 1;
	newHorseCurrent.y = pHorseCurrnet->y + 2;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x - 1;
	newHorseCurrent.y = pHorseCurrnet->y + 2;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x - 1;
	newHorseCurrent.y = pHorseCurrnet->y - 2;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x + 1;
	newHorseCurrent.y = pHorseCurrnet->y - 2;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x + 2;
	newHorseCurrent.y = pHorseCurrnet->y + 1;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x - 2;
	newHorseCurrent.y = pHorseCurrnet->y + 1;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x - 2;
	newHorseCurrent.y = pHorseCurrnet->y - 1;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	newHorseCurrent.x = pHorseCurrnet->x + 2;
	newHorseCurrent.y = pHorseCurrnet->y - 1;
	moveChess(cboardSet, pHorseInit, &newHorseCurrent, chessStep, edgeReached, step + 1);

	free(chessStep);
}

int main(int argc, char *argv[]) {
	Pos cboardSet = { 0 };
	Pos horsePos = { 0 };
	int *chessStep;

	printf_s("Chessboard Size: ");
	scanf_s("%d%d", &cboardSet.x, &cboardSet.y);

	printf_s("Horse Position: ");
	scanf_s("%d%d", &horsePos.x, &horsePos.y);

	printChessBoard(&cboardSet, &horsePos);

	chessStep = malloc(cboardSet.x * cboardSet.y * sizeof(int));
	memset(chessStep, 0, cboardSet.x * cboardSet.y * sizeof(int));

	moveChess(&cboardSet, &horsePos, &horsePos, chessStep, false, 1);
	free(chessStep);

	getchar();
	getchar();

	return STATUS_SUCCESS;
}