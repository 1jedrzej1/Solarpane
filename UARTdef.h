/*
 * myUART.h
 *
 * Created: 2013-12-09 17:53:17
 *  Author: PiotrP
 */ 


#ifndef MYUART_H_
#define MYUART_H_

#define UART_FBEGIN			'#'
#define UART_FEND			'*'

#define UART_F_INIT			'I'
#define UART_F_SILOWNIK		'A'
#define UART_F_SILNIK		'B'
#define UART_F_I12			'C'
#define UART_F_ILAD			'D'
#define UART_F_IROZL		'E'
#define UART_F_VLAD			'F'
#define UART_F_VROZL		'G'
#define UART_F_FOTOR		'H'
#define UART_F_FOTOL		'J'
#define UART_F_FOTOD		'K'
#define UART_F_ERROR		'X'

#define UART_ERR_1			999
#define UART_ERR_2			998
#define UART_ERR_3			997
#define UART_ERR_4			996
#define UART_ERR_5			995
#define UART_ERR_6			994
#define UART_ERR_7			993
#define UART_ERR_8			992

#define UART_ERR_FRAME_FORMAT		UART_ERR_1
#define UART_ERR_WRONG_PARAMETER	UART_ERR_2
#define UART_ERR_ENGINE_INIT		UART_ERR_3
#define UART_ERR_SMART_DETECT		UART_ERR_4
#define UART_ERR_WEIRD_VAL			UART_ERR_5

#define UART_ERR_WRONG_VAL			UART_ERR_1
#define UART_ERR_OUT_OF_RANGE		UART_ERR_1
#define UART_ERR_MAX_VAL_REACHED	UART_ERR_2


//RPI
#define UART_RPI_INIT_AUTO		"#I001*"
#define UART_RPI_INIT_MANUAL	"#I002*"
#define UART_RPI_ACTU_GETPOS	"#A001*"
#define UART_RPI_ACTU_MAN_UP	"#A101*"
#define UART_RPI_ACTU_MAN_DOWN	"#A102*"
#define UART_RPI_ACTU_MAN_STOP	"#A103*"
#define UART_RPI_ENGI_GETPOS	"#B001*"
#define UART_RPI_ENGI_MAN_RIGHT	"#B002*"
#define UART_RPI_ENGI_MAN_LEFT	"#B003*"
#define UART_RPI_I12			"#C001*"
#define UART_RPI_ILAD			"#D001*"
#define UART_RPI_IROZL			"#E001*"
#define UART_RPI_VLAD			"#F001*"
#define UART_RPI_VROZL			"#G001*"
#define UART_RPI_FOTO_R			"#H001*"
#define UART_RPI_FOTO_L			"#J001*"
#define UART_RPI_FOTO_D			"#K001*"

#endif /* MYUART_H_ */