-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Servidor: localhost
-- Tiempo de generación: 08-07-2023 a las 18:28:53
-- Versión del servidor: 10.4.28-MariaDB
-- Versión de PHP: 8.2.4

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de datos: `webscythe`
--

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `logros`
--

CREATE TABLE `logros` (
  `IDLogro` varchar(30) NOT NULL,
  `Descripcion` varchar(1000) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Volcado de datos para la tabla `logros`
--

INSERT INTO `logros` (`IDLogro`, `Descripcion`) VALUES
('GrassHopper', '¡Has saltado más de 30 veces!'),
('Sleeping', 'Te has quedado dormido durante 5 min...');

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `partidas`
--

CREATE TABLE `partidas` (
  `IDKeyPartida` bigint(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Volcado de datos para la tabla `partidas`
--

INSERT INTO `partidas` (`IDKeyPartida`) VALUES
(595250080313);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `partidas_usuarios`
--

CREATE TABLE `partidas_usuarios` (
  `ID` int(11) NOT NULL,
  `IDKeyPartida` bigint(12) NOT NULL,
  `Usuario` varchar(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `partida_logros`
--

CREATE TABLE `partida_logros` (
  `ID` int(11) NOT NULL,
  `IDKeyPartida` bigint(12) NOT NULL,
  `IDLogro` varchar(30) NOT NULL,
  `FechaConseguido` varchar(30) NOT NULL,
  `TiempoTotal` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Volcado de datos para la tabla `partida_logros`
--

INSERT INTO `partida_logros` (`ID`, `IDKeyPartida`, `IDLogro`, `FechaConseguido`, `TiempoTotal`) VALUES
(24, 595250080313, 'GrassHopper', '08/07/2023 13:30:46', 0.062222);

-- --------------------------------------------------------

--
-- Estructura de tabla para la tabla `usuarios`
--

CREATE TABLE `usuarios` (
  `Usuario` varchar(30) NOT NULL,
  `Contraseña` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Índices para tablas volcadas
--

--
-- Indices de la tabla `logros`
--
ALTER TABLE `logros`
  ADD PRIMARY KEY (`IDLogro`);

--
-- Indices de la tabla `partidas`
--
ALTER TABLE `partidas`
  ADD PRIMARY KEY (`IDKeyPartida`);

--
-- Indices de la tabla `partidas_usuarios`
--
ALTER TABLE `partidas_usuarios`
  ADD PRIMARY KEY (`ID`),
  ADD KEY `Usuario_FK` (`Usuario`),
  ADD KEY `IDKeyPartida_FK` (`IDKeyPartida`);

--
-- Indices de la tabla `partida_logros`
--
ALTER TABLE `partida_logros`
  ADD PRIMARY KEY (`ID`),
  ADD UNIQUE KEY `Partida_Logro` (`IDKeyPartida`,`IDLogro`),
  ADD KEY `IDLogro_FK` (`IDLogro`);

--
-- Indices de la tabla `usuarios`
--
ALTER TABLE `usuarios`
  ADD PRIMARY KEY (`Usuario`);

--
-- AUTO_INCREMENT de las tablas volcadas
--

--
-- AUTO_INCREMENT de la tabla `partidas_usuarios`
--
ALTER TABLE `partidas_usuarios`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de la tabla `partida_logros`
--
ALTER TABLE `partida_logros`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=25;

--
-- Restricciones para tablas volcadas
--

--
-- Filtros para la tabla `partidas_usuarios`
--
ALTER TABLE `partidas_usuarios`
  ADD CONSTRAINT `IDKeyPartida_FK` FOREIGN KEY (`IDKeyPartida`) REFERENCES `partidas` (`IDKeyPartida`),
  ADD CONSTRAINT `Usuario_FK` FOREIGN KEY (`Usuario`) REFERENCES `usuarios` (`Usuario`);

--
-- Filtros para la tabla `partida_logros`
--
ALTER TABLE `partida_logros`
  ADD CONSTRAINT `IDKey_Partida_FK` FOREIGN KEY (`IDKeyPartida`) REFERENCES `partidas` (`IDKeyPartida`),
  ADD CONSTRAINT `IDLogro_FK` FOREIGN KEY (`IDLogro`) REFERENCES `logros` (`IDLogro`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
